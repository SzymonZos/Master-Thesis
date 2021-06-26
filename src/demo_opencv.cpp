#ifdef BUILD_OPENCV

#include "dwt_2d.hpp"
#include "dwt_helpers.hpp"
#include "filter_luts.hpp"

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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

bool imWrite(auto&&... params) {
    try {
        return cv::imwrite(std::forward<decltype(params)>(params)...);
    } catch (const cv::Exception& ex) {
        std::cerr << "Exception converting image ("
                  << std::get<0>(std::forward_as_tuple(params...))
                  << "): " << ex.what() << "\n";
    } catch (...) { std::cerr << "Unknown exception\n"; }
    return false;
}

void opencvTesting() {
    cv::Mat mat(480, 640, CV_8UC4); // Create a matrix with alpha channel
    paintAlphaMat(mat);
    std::vector<int> compression_params(2);
    compression_params[0] = cv::IMWRITE_PNG_COMPRESSION;
    compression_params[1] = 9;
    imWrite("alpha.png", mat, compression_params);
    imWrite("beta.jp2", mat);
}
} // namespace
} // namespace mgr
#endif

namespace mgr {
void demo_opencv() {
#ifdef BUILD_OPENCV
    cv::Mat img = cv::imread("../../img/lena.png", cv::IMREAD_GRAYSCALE);
    if (img.empty()) {
        std::cout << "!!! Failed imread(): image not found\n";
    }
    //    cv::cvtColor(img, img, cv::COLOR_RGB2GRAY);
    std::cout << img.size << "\n";
    cv::Mat img_f;
    //    img.convertTo(img_f, CV_32F, 1. / 255.);
    img.convertTo(img_f, CV_32F);
    cv::Mat1f dupa(img);
    auto [rows_out,
          cols_out] = get_out_rows_cols(static_cast<std::size_t>(img_f.rows),
                                        static_cast<std::size_t>(img_f.cols),
                                        lut_bior2_2_f.size(),
                                        dwt_mode::_2d);
    cv::Mat out(static_cast<int>(rows_out),
                static_cast<int>(cols_out),
                img_f.type());
    std::cout << img.type() << "\n";
    dwt_2d(img_f.ptr<float>(0),
           static_cast<std::size_t>(img_f.rows),
           static_cast<std::size_t>(img_f.cols),
           lut_bior2_2_f.data(),
           lut_bior2_2_f.size(),
           out.ptr<float>(0),
           padding_mode::symmetric);
    cv::normalize(out, out, 0., 1., cv::NORM_MINMAX, CV_32F);
    cv::Mat out_u8;
    out.convertTo(out_u8, CV_8U, 255);
    imWrite("../../img/lena_dwt.bmp", out_u8);
    []() {
    }();
#endif
}
} // namespace mgr
