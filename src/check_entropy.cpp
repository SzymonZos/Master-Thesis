#include "check_entropy.hpp"

#include "dwt_2d.hpp"
#include "dwt_helpers.hpp"
#include "entropy.hpp"
#include "filesystem.hpp"
#include "filter_luts.hpp"
#include "parallel_for.hpp"
#include "queue.hpp"

#include "Timer.hpp"
#include "opencv.hpp"

#include <version>
#ifdef __cpp_lib_execution
#include <execution>
#endif
#include <iostream>

namespace mgr {
namespace {
int orig_size = 0;

template<typename T, typename U>
T memoryless_entropy(const U* restrict arr, int rows, int cols) {
    return mgr::memoryless_entropy<T>(arr,
                                      static_cast<std::size_t>(rows) *
                                          static_cast<std::size_t>(cols));
}

template<typename T>
T memoryless_entropy(const cv::Mat& img) {
    T factor = static_cast<T>(img.rows * img.cols) / static_cast<T>(orig_size);
    return factor * memoryless_entropy<T>(img.data, img.rows, img.cols);
}

void normalize_img(cv::Mat& img) {
    cv::normalize(img, img, 0., 1., cv::NORM_MINMAX, CV_32F);
    img.convertTo(img, CV_8U, 255);
}

constexpr auto n_queue = detail::get_queue_size();
} // namespace
} // namespace mgr

namespace mgr {
void check_entropy_threads(const std::string& path, unsigned int n_threads) {
    cv::Mat img = read_img(path, cv::IMREAD_COLOR);
    orig_size = img.rows * img.cols;
    std::vector<cv::Mat> channels(3);
    cv::split(img, channels);

    auto par_queue = [&channels](std::size_t i) mutable {
        auto out = channels;
        float entropy = 0.F;
        float scale = 0.F;
        for (auto cb : dwt_queue<float>[i]) {
            for (auto& channel : out) {
                if (cb == no_dwt_2d<float, float>) {
                    scale = 2.F;
                    break;
                }
                if (cb == dwt_2d<float, float>) {
                    scale = 1.6F;
                    auto out_hl = dwt_2d_img_wrapper(channel,
                                                     lut_bior2_2_h_f,
                                                     dwt_2d_rows<float, float>,
                                                     padding_mode::symmetric);
                    out_hl = dwt_2d_img_wrapper(out_hl,
                                                lut_bior2_2_f,
                                                dwt_2d_cols<float, float>,
                                                padding_mode::symmetric);
                    normalize_img(out_hl);
                    entropy += memoryless_entropy<float>(out_hl);
                    auto out_lh = dwt_2d_img_wrapper(channel,
                                                     lut_bior2_2_f,
                                                     dwt_2d_rows<float, float>,
                                                     padding_mode::symmetric);
                    out_lh = dwt_2d_img_wrapper(out_lh,
                                                lut_bior2_2_h_f,
                                                dwt_2d_cols<float, float>,
                                                padding_mode::symmetric);
                    normalize_img(out_lh);
                    entropy += memoryless_entropy<float>(out_lh);
                }
                auto out_h = dwt_2d_img_wrapper(channel,
                                                lut_bior2_2_h_f,
                                                cb,
                                                padding_mode::symmetric);
                normalize_img(out_h);
                entropy += memoryless_entropy<float>(out_h);

                channel = dwt_2d_img_wrapper(channel,
                                             lut_bior2_2_f,
                                             cb,
                                             padding_mode::symmetric);
            }
        }
        for (auto& channel : out) {
            normalize_img(channel);
            cv::Mat median;
            cv::medianBlur(channel, median, 3);
            cv::Mat abs;
            cv::absdiff(channel, median, abs);
            entropy += scale * memoryless_entropy<float>(abs);
        }
        return entropy;
    };
    std::string timer_path = "threads_" + std::to_string(n_threads) + ".txt";
    Timer<std::chrono::milliseconds> t{timer_path};
    auto entropies = parallel_for(n_threads, n_queue, std::move(par_queue));
    auto min = std::distance(
        entropies.begin(),
        std::min_element(entropies.begin(), entropies.end()));
    std::cout << min << "\n";
}

void check_entropy_exec_policies(const std::string& path) {
#ifdef __cpp_lib_execution
    std::vector<float> entropies(detail::get_queue_size());
    alignas(64) std::atomic<std::size_t> i{0};

    cv::Mat img = read_img(path, cv::IMREAD_COLOR);
    orig_size = img.rows * img.cols;
    std::vector<cv::Mat> channels(3);
    cv::split(img, channels);

    Timer<std::chrono::milliseconds> t{"exec_policies_times.txt"};
    std::for_each(
        std::execution::par_unseq,
        dwt_queue<float>.begin(),
        dwt_queue<float>.end(),
        [&entropies, &i, &channels](const auto& cbs) {
            auto out = channels;
            float entropy = 0.F;
            float scale = 0.F;
            for (auto cb : cbs) {
                for (auto& channel : out) {
                    if (cb == no_dwt_2d<float, float>) {
                        scale = 2.F;
                        break;
                    }
                    if (cb == dwt_2d<float, float>) {
                        scale = 1.6F;
                        auto out_hl = dwt_2d_img_wrapper(
                            channel,
                            lut_bior2_2_h_f,
                            dwt_2d_rows<float, float>,
                            padding_mode::symmetric);
                        out_hl = dwt_2d_img_wrapper(out_hl,
                                                    lut_bior2_2_f,
                                                    dwt_2d_cols<float, float>,
                                                    padding_mode::symmetric);
                        normalize_img(out_hl);
                        entropy += memoryless_entropy<float>(out_hl);
                        auto out_lh = dwt_2d_img_wrapper(
                            channel,
                            lut_bior2_2_f,
                            dwt_2d_rows<float, float>,
                            padding_mode::symmetric);
                        out_lh = dwt_2d_img_wrapper(out_lh,
                                                    lut_bior2_2_h_f,
                                                    dwt_2d_cols<float, float>,
                                                    padding_mode::symmetric);
                        normalize_img(out_lh);
                        entropy += memoryless_entropy<float>(out_lh);
                    }
                    auto out_h = dwt_2d_img_wrapper(channel,
                                                    lut_bior2_2_h_f,
                                                    cb,
                                                    padding_mode::symmetric);
                    normalize_img(out_h);
                    entropy += memoryless_entropy<float>(out_h);

                    channel = dwt_2d_img_wrapper(channel,
                                                 lut_bior2_2_f,
                                                 cb,
                                                 padding_mode::symmetric);
                }
            }
            for (auto& channel : out) {
                normalize_img(channel);
                cv::Mat median;
                cv::medianBlur(channel, median, 3);
                cv::Mat abs;
                cv::absdiff(channel, median, abs);
                entropy += scale * memoryless_entropy<float>(abs);
            }
            entropies[i++] = entropy;
        });
    auto min = std::distance(
        entropies.begin(),
        std::min_element(entropies.begin(), entropies.end()));
    std::cout << min << "\n";
#endif
}
} // namespace mgr
