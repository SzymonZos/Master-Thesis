#include "demo_opencv.hpp"

#include "dwt_2d.hpp"
#include "dwt_helpers.hpp"
#include "entropy.hpp"
#include "filesystem.hpp"
#include "filter_luts.hpp"
#include "parallel_for.hpp"
#include "queue.hpp"

#include "Timer.hpp"
#include <iostream>

#ifdef BUILD_OPENCV
#include "opencv.hpp"
#include <opencv2/imgproc.hpp>

#include "dwt.hpp"

#define DEBUG 0

namespace mgr {
namespace {
void paintAlphaMat(cv::Mat& mat) {
    CV_Assert(mat.channels() == 4);
    for (int i = 0; i < mat.rows; ++i) {
        for (int j = 0; j < mat.cols; ++j) {
            auto& bgra = mat.at<cv::Vec4b>(i, j);
            // Blue
            bgra[0] = UCHAR_MAX;
            // Green
            bgra[1] = cv::saturate_cast<uchar>((float(mat.cols - j)) /
                                               (float(mat.cols)) * UCHAR_MAX);
            // Red
            bgra[2] = cv::saturate_cast<uchar>((float(mat.rows - i)) /
                                               (float(mat.rows)) * UCHAR_MAX);
            // Alpha
            bgra[3] = cv::saturate_cast<uchar>(0.5 * (bgra[1] + bgra[2]));
        }
    }
}

[[maybe_unused]] void opencvTesting() {
    cv::Mat mat(480, 640, CV_8UC4); // Create a matrix with alpha channel
    paintAlphaMat(mat);
    std::vector<int> compression_params(2);
    compression_params[0] = cv::IMWRITE_PNG_COMPRESSION;
    compression_params[1] = 9;
    imwrite("alpha.png", mat, compression_params);
    imwrite("beta.jp2", mat);
}

template<typename T, typename U>
T memoryless_entropy(const U* restrict arr, int rows, int cols) {
    return mgr::memoryless_entropy<T>(arr,
                                      static_cast<std::size_t>(rows) *
                                          static_cast<std::size_t>(cols));
}

template<typename T>
T memoryless_entropy(const cv::Mat& img) {
    return memoryless_entropy<T>(img.data, img.rows, img.cols);
}

void normalize_img(cv::Mat& img) {
    cv::normalize(img, img, 0., 1., cv::NORM_MINMAX, CV_32F);
    img.convertTo(img, CV_8U, 255);
}

constexpr auto n_queue = detail::get_queue_size();
const auto n_threads = std::thread::hardware_concurrency();
} // namespace
} // namespace mgr

namespace mgr {
void demo_opencv(const std::string& path) {
    cv::Mat img = read_img(path, cv::IMREAD_GRAYSCALE);
    cv::Mat out = dwt_2d_img_wrapper(img,
                                     lut_bior2_2_f,
                                     dwt_2d<float, float>,
                                     padding_mode::symmetric);
    fs::path input_path{path};
    fs::path tmp_path = get_path_in_same_dir(input_path, "tmp");
    fs::create_directory(tmp_path);
    tmp_path /= append_to_filename(input_path, "_dwt");
    save_img(tmp_path.string(), out);
}

void demo_opencv_queue(const std::string& path) {
    cv::Mat img = read_img(path, cv::IMREAD_GRAYSCALE);
    fs::path input_path{path};
    fs::path tmp_path = get_path_in_same_dir(input_path, "tmp");
    fs::create_directory(tmp_path);

    Timer<std::chrono::milliseconds> t{};
    for (std::size_t i{}; const auto& cbs : dwt_queue<float>) {
        using namespace std::string_literals;
        auto out = img;
        for (auto cb : cbs) {
            out = dwt_2d_img_wrapper(out,
                                     lut_bior2_2_f,
                                     cb,
                                     padding_mode::symmetric);
        }
        tmp_path /= append_to_filename(input_path,
                                       append_iter_to_name("_dwt_", i++));
        save_img(tmp_path.string(), out);
        tmp_path.remove_filename();
    }
}

void demo_opencv_parallel_queue(const std::string& path) {
    cv::Mat img = read_img(path, cv::IMREAD_GRAYSCALE);
    fs::path input_path{path};
    fs::path tmp_path = get_path_in_same_dir(input_path, "tmp_2");
    fs::create_directory(tmp_path);

    auto par_queue = [&img, &input_path, tmp_path](std::size_t i) mutable {
        auto out = img;
        for (auto cb : dwt_queue<float>[i]) {
            out = dwt_2d_img_wrapper(out,
                                     lut_bior2_2_f,
                                     cb,
                                     padding_mode::symmetric);
        }
        tmp_path /= append_to_filename(input_path,
                                       append_iter_to_name("_dwt_", i));
        save_img(tmp_path.string(), out);
        tmp_path.remove_filename();
    };
    Timer<std::chrono::milliseconds> t{};
    parallel_for(n_threads, n_queue, std::move(par_queue));
}

void demo_opencv_entropy(const std::string& path) {
    cv::Mat img = read_img(path, cv::IMREAD_GRAYSCALE);
#if not DEBUG
    auto par_queue = [&img](std::size_t i) mutable {
        auto out = img;
        float entropy = 0.F;
        for (auto cb : dwt_queue<float>[i + 1]) {
            if (cb == no_dwt_2d<float, float>) {
                break;
            }
            if (cb == dwt_2d<float, float>) {
                auto out_hl = dwt_2d_img_wrapper(out,
                                                 lut_bior2_2_h_f,
                                                 dwt_2d_rows<float, float>,
                                                 padding_mode::symmetric);
                out_hl = dwt_2d_img_wrapper(out_hl,
                                            lut_bior2_2_f,
                                            dwt_2d_cols<float, float>,
                                            padding_mode::symmetric);
                normalize_img(out_hl);
                entropy += memoryless_entropy<float>(out_hl);
                auto out_lh = dwt_2d_img_wrapper(out,
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
            auto out_h = dwt_2d_img_wrapper(out,
                                            lut_bior2_2_h_f,
                                            cb,
                                            padding_mode::symmetric);
            normalize_img(out_h);
            entropy += memoryless_entropy<float>(out_h);

            out = dwt_2d_img_wrapper(out,
                                     lut_bior2_2_f,
                                     cb,
                                     padding_mode::symmetric);
        }
        normalize_img(out);
        cv::Mat median;
        cv::medianBlur(out, median, 3);
        cv::Mat abs;
        cv::absdiff(out, median, abs);
        entropy += memoryless_entropy<float>(abs);
        return entropy;
    };
    Timer<std::chrono::milliseconds> t{};
    auto entropies = parallel_for(n_threads,
                                  n_queue - 1,
                                  std::move(par_queue));
    auto min = std::distance(
        entropies.begin(),
        std::min_element(entropies.begin(), entropies.end()));
    std::cout << min + 1 << "\n";
#else
    cv::Mat median;
    cv::medianBlur(img, median, 3);
    imwrite("median.png", median);
    cv::Mat out;
    cv::absdiff(img, median, out);
    imwrite("out.png", out);
    std::cout << memoryless_entropy<float>(img) << "\n"
              << memoryless_entropy<float>(out) << "\n";
    auto out_l = dwt_2d_img_wrapper(out,
                                    lut_bior2_2_f,
                                    dwt_2d_rows<float, float>,
                                    padding_mode::symmetric);
    save_img("out_l.png", out_l);
    auto out_h = dwt_2d_img_wrapper(img,
                                    lut_bior2_2_h_f,
                                    dwt_2d_rows<float, float>,
                                    padding_mode::symmetric);
    save_img("out_h.png", out_h);
    std::cout << memoryless_entropy<float>(out_l) +
                     memoryless_entropy<float>(out_h)
              << "\n";
    float dupa[6] = {55, 234, 70, 21, 88, 37};
    float kupa[6] = {};
    dwt_1d(dupa,
           6,
           lut_bior2_2_h_f.data(),
           lut_bior2_2_h_f.size(),
           kupa,
           padding_mode::symmetric);
    for (std::size_t i{}; i < 6; i++) {
        std::cout << kupa[i] << " ";
    }
    std::cout << "\n";
#endif
}

void demo_opencv_parallel_queue_rgb(const std::string& path) {
    cv::Mat img = read_img(path, cv::IMREAD_COLOR);
    std::vector<cv::Mat> channels(3);
    cv::split(img, channels);
    fs::path input_path{path};
    fs::path tmp_path = get_path_in_same_dir(input_path, "tmp_3");
    fs::create_directory(tmp_path);

    auto par_queue =
        [&channels, &input_path, tmp_path](std::size_t i) mutable {
            auto out_channels = channels;
            for (auto cb : dwt_queue<float>[i]) {
                for (auto& channel : out_channels) {
                    channel = dwt_2d_img_wrapper(channel,
                                                 lut_bior2_2_f,
                                                 cb,
                                                 padding_mode::symmetric);
                }
            }
            tmp_path /= append_to_filename(input_path,
                                           append_iter_to_name("_dwt_", i));
            cv::Mat out_img;
            cv::merge(out_channels, out_img);
            save_img(tmp_path.string(), out_img);
            tmp_path.remove_filename();
        };
    Timer<std::chrono::milliseconds> t{};
    parallel_for(n_threads, n_queue, std::move(par_queue));
}
} // namespace mgr
#else
namespace mgr {
void demo_opencv(const std::string& path) {}
void demo_opencv_queue(const std::string& path) {}
void demo_opencv_parallel_queue(const std::string& path) {}
} // namespace mgr
#endif
