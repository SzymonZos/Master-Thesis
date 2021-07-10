#ifndef JPEG2000_OPENCV_HPP
#define JPEG2000_OPENCV_HPP

#include "dwt_2d.hpp"
#include "dwt_helpers.hpp"

#include <array>
#include <string>
#include <tuple>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>

namespace mgr {
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

cv::Mat read_img(const std::string& path, cv::ImreadModes img_mode) {
    cv::Mat img = cv::imread(path, img_mode);
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
} // namespace mgr

#endif // JPEG2000_OPENCV_HPP
