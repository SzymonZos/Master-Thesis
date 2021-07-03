#include "demo_opencv.hpp"

#include "dwt_2d.hpp"
#include "dwt_helpers.hpp"
#include "filter_luts.hpp"
#include "parallel_for.hpp"
#include "queue.hpp"
#include "filesystem.hpp"

#include "Timer.hpp"
#include <iostream>
#include <thread>
#include <vector>

#ifdef BUILD_OPENCV
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>

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

bool imwrite(auto&&... params) {
    try {
        return cv::imwrite(std::forward<decltype(params)>(params)...);
    } catch (const cv::Exception& ex) {
        std::cerr << "Exception converting image ("
                  << std::get<0>(std::forward_as_tuple(params...))
                  << "): " << ex.what() << "\n";
    } catch (...) { std::cerr << "Unknown exception\n"; }
    return false;
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

cv::Mat read_img(const std::string& path) {
    cv::Mat img = cv::imread(path, cv::IMREAD_GRAYSCALE);
    if (img.empty()) {
        throw std::runtime_error("cv::imread() failed: image not found");
    }
    img.convertTo(img, CV_32F);
    return img;
}

void save_img(const std::string& path, cv::Mat& img) {
    cv::normalize(img, img, 0., 1., cv::NORM_MINMAX, CV_32F);
    img.convertTo(img, CV_8U, 255);
    imwrite(path, img);
}

template<typename T, std::size_t N>
cv::Mat dwt_2d_img_wrapper(const cv::Mat& input,
                           const std::array<T, N>& filter,
                           dwt_2d_cb<T, T> dwt_cb,
                           padding_mode mode) {
    auto [rows_out,
          cols_out] = get_out_rows_cols(static_cast<std::size_t>(input.rows),
                                        static_cast<std::size_t>(input.cols),
                                        filter.size(),
                                        dwt_cb);
    cv::Mat output(static_cast<int>(rows_out),
                   static_cast<int>(cols_out),
                   input.type());
    dwt_cb(input.ptr<T>(0),
           static_cast<std::size_t>(input.rows),
           static_cast<std::size_t>(input.cols),
           filter.data(),
           filter.size(),
           output.ptr<T>(0),
           mode);
    return output;
}
} // namespace
} // namespace mgr

namespace mgr {
void demo_opencv(const std::string& path) {
    cv::Mat img = read_img(path);
    cv::Mat out = dwt_2d_img_wrapper(img,
                                     lut_bior2_2_f,
                                     dwt_2d<float, float>,
                                     padding_mode::symmetric);
    fs::path input_path{path};
    fs::path tmp_path{input_path.parent_path() / "tmp"};
    fs::create_directory(tmp_path);
    tmp_path /= ((input_path.stem() += "_dwt") += input_path.extension());
    save_img(tmp_path.string(), out);
}

void demo_opencv_queue(const std::string& path) {
    using namespace std::string_literals;
    cv::Mat img = read_img(path);
    fs::path input_path{path};
    fs::path tmp_path{input_path.parent_path() / "tmp"};
    fs::create_directory(tmp_path);

    Timer<std::chrono::milliseconds> t{};
    for (std::size_t i{}; const auto& cbs : mgr::dwt_queue<float>) {
        auto out = img;
        for (auto cb : cbs) {
            out = dwt_2d_img_wrapper(out,
                                     lut_bior2_2_f,
                                     cb,
                                     mgr::padding_mode::symmetric);
        }
        std::string ext = "_dwt_"s + std::to_string(i++);
        tmp_path /= ((input_path.stem() += ext) += input_path.extension());
        save_img(tmp_path.string(), out);
        tmp_path.remove_filename();
    }
}

void demo_opencv_parallel_queue(const std::string& path) {
    constexpr auto n_queue = mgr::detail::get_queue_size();
    const auto n_threads = std::thread::hardware_concurrency();

    using namespace std::string_literals;
    cv::Mat img = read_img(path);
    fs::path input_path{path};
    fs::create_directory(input_path.parent_path() / "tmp_2");

    Timer<std::chrono::milliseconds> t{};
    auto par_queue = [&img, &input_path](std::size_t i) mutable {
        auto out = img;
        for (auto cb : dwt_queue<float>[i]) {
            out = dwt_2d_img_wrapper(out,
                                     lut_bior2_2_f,
                                     cb,
                                     padding_mode::symmetric);
        }
        std::string ext = "_dwt_"s + std::to_string(i);
        fs::path tmp_path{input_path.parent_path() / "tmp_2"};
        tmp_path /= (input_path.stem() + ext + input_path.extension());
        save_img(tmp_path.string(), out);
    };
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
