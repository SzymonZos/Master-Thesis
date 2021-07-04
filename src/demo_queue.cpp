#include "demo_queue.hpp"

#include "dwt_helpers.hpp"
#include "filter_luts.hpp"
#include "matrix.hpp"
#include "parallel_for.hpp"
#include "queue.hpp"

#include "Timer.hpp"

#include <array>
#include <iostream>
#include <thread>
#include <vector>

#ifndef __clang__
#include <execution>
#endif

namespace mgr {
namespace {
const matrix<float> test_mat{{1, 2, 3, 4, 5, 5, 6, 7, 8, 9}, 5, 2};

auto demo_sequential_() {
    std::vector<matrix<float>> vec;
    vec.reserve(detail::get_queue_size());
    Timer<std::chrono::nanoseconds> t{};
    for (const auto& cbs : dwt_queue<float>) {
        auto out_mat = test_mat;
        for (auto cb : cbs) {
            out_mat = dwt_2d_wrapper(out_mat,
                                     lut_bior2_2_f,
                                     cb,
                                     padding_mode::symmetric);
        }
        vec.emplace_back(std::move(out_mat));
    }
    return vec;
}

#ifndef __clang__
auto demo_parallel_() {
    std::vector<matrix<float>> vec(detail::get_queue_size());
    alignas(64) std::atomic<std::size_t> i{0};
    Timer<std::chrono::nanoseconds> t{};
    std::for_each(std::execution::par_unseq,
                  dwt_queue<float>.begin(),
                  dwt_queue<float>.end(),
                  [&vec, &i](const auto& cbs) {
                      auto out_mat = test_mat;
                      for (auto cb : cbs) {
                          out_mat = dwt_2d_wrapper(out_mat,
                                                   lut_bior2_2_f,
                                                   cb,
                                                   padding_mode::symmetric);
                      }
                      vec[i++] = std::move(out_mat);
                  });
    return vec;
}
#endif

auto demo_my_parallel_() {
    constexpr auto n_queue = detail::get_queue_size();
    const auto n_threads = std::thread::hardware_concurrency();
    auto par_queue = [](std::size_t i) -> matrix<float> {
        auto out_mat = test_mat;
        for (auto cb : dwt_queue<float>[i]) {
            out_mat = dwt_2d_wrapper(out_mat,
                                     lut_bior2_2_f,
                                     cb,
                                     padding_mode::symmetric);
        }
        return out_mat;
    };

    Timer<std::chrono::nanoseconds> t{};
    auto vec = parallel_for(n_threads, n_queue, std::move(par_queue));
    return vec;
}
} // namespace

void demo_sequential() {
    std::cout << "SEQUENTIAL VERSION\n";
    auto vec_seq = demo_sequential_();
    for (auto&& mat : vec_seq) {
        std::cout << mat << "\n\n";
    }
}

void demo_parallel() {
    std::cout << "\n\nPARALLEL VERSION\n";
#ifndef __clang__
    auto vec_par = demo_parallel_();
    for (auto&& mat : vec_par) {
        std::cout << mat << "\n\n";
    }
#endif
}

void demo_my_parallel() {
    std::cout << "\n\nMY PARALLEL VERSION\n";
    auto vec_my_par = demo_my_parallel_();
    for (auto&& mat : vec_my_par) {
        std::cout << mat << "\n\n";
    }
}

} // namespace mgr

#if 0
std::cout << std::boolalpha << (vec_seq == vec_my_par) << "\n";
#endif
