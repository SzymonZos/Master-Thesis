#include <algorithm>
#include <array>
#include <iostream>
#include <new>
#include <numeric>
#include <opencv2/imgcodecs.hpp>
#include <thread>

namespace mgr {

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

alignas(64) std::atomic<std::size_t> index;
constexpr std::size_t queueSize = 100;

struct atrocity {
    // data
};

constexpr void fillAtrocities() {
    // generate queue array by filling members with right values
}

std::array<std::thread::id, queueSize> queue{};

void threadTesting() {
    //    std::iota(queue.begin(), queue.end(), -100);
    std::vector<std::thread> threads;
    const auto noThreads = std::thread::hardware_concurrency();
    threads.reserve(noThreads);

    for (std::size_t i{}; i < noThreads; i++) {
        threads.emplace_back([]() {
            while (index < queueSize) {
                using namespace std::chrono_literals;
                queue[++index] = std::this_thread::get_id();
                std::this_thread::sleep_for(1ms);
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    std::unordered_map<std::thread::id, std::size_t> counter;
    for (std::size_t i{}; i < queue.size(); i++) {
        std::cout << "[" << i << "]: " << queue[i] << "\n";
        counter[queue[i]]++;
    }
    for (const auto& [id, count] : counter) {
        std::cout << id << ": " << count << "\n";
    }
}

} // namespace mgr

int main() {
    mgr::threadTesting();
    return 0;
}
