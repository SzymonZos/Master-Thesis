#include <algorithm>
#include <array>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <vector>

#include "dwt.hpp"
#include "luts.hpp"
#include "matrix.hpp"

namespace mgr {

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
    //    mgr::threadTesting();
    for (auto x : mgr::detail::lut_db2<float>) {
        std::cout << x << " ";
    }
    std::cout << "\n";
    for (auto x : mgr::lut_db2_f) {
        std::cout << x << " ";
    }
    std::array<float, 4> in{1, 2, 3, 4};
    std::vector<float> out(128);

    mgr::downsampling_convolution(in.data(),
                                  in.size(),
                                  mgr::lut_db2_f.data(),
                                  mgr::lut_db2_f.size(),
                                  out.data(),
                                  mgr::padding_mode::symmetric);
    std::cout << "\n";
    for (std::size_t i{}; i < 3; i++) {
        std::cout << out[i] << " ";
    }
    std::cout << "\n";
    std::array<float, 10> in_2d{1, 2, 3, 4, 5, 5, 6, 7, 8, 9};
    for (std::size_t i{}; i < 2; i++) {
        mgr::downsampling_convolution(in_2d.data() + i * 5,
                                      5,
                                      mgr::lut_bior2_2_f.data(),
                                      mgr::lut_bior2_2_f.size(),
                                      out.data() + i * 5,
                                      mgr::padding_mode::symmetric);
    }
    for (std::size_t i{}; i < 10; i++) {
        std::cout << out[i] << " ";
    }
    std::cout << "\n";
    mgr::matrix<float> test_mat{{1, 2, 3, 4, 5, 5, 6, 7, 8, 9}, 2, 5};
    for (std::size_t i{}; i < test_mat.n_rows(); i++) {
        mgr::downsampling_convolution(test_mat.get_row(i),
                                      test_mat.n_cols(),
                                      mgr::lut_bior2_2_f.data(),
                                      mgr::lut_bior2_2_f.size(),
                                      out.data() + i * test_mat.n_cols(),
                                      mgr::padding_mode::symmetric);
    }
    for (std::size_t i{}; i < 10; i++) {
        std::cout << out[i] << " ";
    }
    for (std::size_t i{}; i < test_mat.n_cols(); i++) {
        mgr::downsampling_convolution(test_mat.get_col(i),
                                      test_mat.n_rows(),
                                      mgr::lut_bior2_2_f.data(),
                                      mgr::lut_bior2_2_f.size(),
                                      out.data() + i * (test_mat.n_rows() + 1),
                                      mgr::padding_mode::symmetric);
    }
    std::cout << "\n\n";
    for (std::size_t i{}; i < 15; i++) {
        if (i && i % 3 == 0) {
            std::cout << "\n";
        }
        std::cout << out[i] << " ";
    }
    std::cout << "\n\n";

    for (std::size_t i{}; i < test_mat.n_cols(); i++) {
        mgr::downsampling_convolution(test_mat.data() + i,
                                      test_mat.n_rows(),
                                      mgr::lut_bior2_2_f.data(),
                                      mgr::lut_bior2_2_f.size(),
                                      out.data() + i,
                                      mgr::padding_mode::symmetric,
                                      test_mat.n_cols());
    }
    for (std::size_t i{}; i < 15; i++) {
        if (i && i % 5 == 0) {
            std::cout << "\n";
        }
        std::cout << out[i] << " ";
    }
    std::cout << "\n";

    std::vector<float> out_2(128);
    for (std::size_t i{}; i < test_mat.n_rows(); i++) {
        mgr::downsampling_convolution(test_mat.get_row(i),
                                      test_mat.n_cols(),
                                      mgr::lut_bior2_2_f.data(),
                                      mgr::lut_bior2_2_f.size(),
                                      out_2.data() + i * test_mat.n_cols(),
                                      mgr::padding_mode::symmetric);
    }
    for (std::size_t i{}; i < test_mat.n_cols(); i++) {
        mgr::downsampling_convolution(out_2.data() + i,
                                      test_mat.n_rows(),
                                      mgr::lut_bior2_2_f.data(),
                                      mgr::lut_bior2_2_f.size(),
                                      out.data() + i,
                                      mgr::padding_mode::symmetric,
                                      test_mat.n_cols());
    }
    for (std::size_t i{}; i < 15; i++) {
        if (i && i % 5 == 0) {
            std::cout << "\n";
        }
        std::cout << out[i] << " ";
    }
    std::cout << "\n";
    return 0;
}
