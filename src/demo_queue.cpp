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
const mgr::matrix<float> test_mat{{1, 2, 3, 4, 5, 5, 6, 7, 8, 9}, 5, 2};

auto demo_sequential_() {
    std::vector<mgr::matrix<float>> vec;
    vec.reserve(mgr::detail::get_queue_size());
    Timer<std::chrono::nanoseconds> t{};
    for (const auto& cbs : mgr::dwt_queue<float>) {
        auto out_mat = test_mat;
        for (auto cb : cbs) {
            out_mat = mgr::dwt_2d_wrapper(out_mat,
                                          mgr::lut_bior2_2_f,
                                          cb,
                                          mgr::padding_mode::symmetric);
        }
        vec.emplace_back(std::move(out_mat));
    }
    return vec;
}

#ifndef __clang__
auto demo_parallel_() {
    std::vector<mgr::matrix<float>> vec(mgr::detail::get_queue_size());
    alignas(64) std::atomic<std::size_t> i{0};
    Timer<std::chrono::nanoseconds> t{};
    std::for_each(std::execution::par_unseq,
                  mgr::dwt_queue<float>.begin(),
                  mgr::dwt_queue<float>.end(),
                  [&vec, &i](const auto& cbs) {
                      auto out_mat = test_mat;
                      for (auto cb : cbs) {
                          out_mat = mgr::dwt_2d_wrapper(
                              out_mat,
                              mgr::lut_bior2_2_f,
                              cb,
                              mgr::padding_mode::symmetric);
                      }
                      vec[i++] = std::move(out_mat);
                  });
    return vec;
}
#endif

auto demo_my_parallel_() {
    constexpr auto n_queue = mgr::detail::get_queue_size();
    const auto n_threads = std::thread::hardware_concurrency();

    std::vector<mgr::matrix<float>> vec(n_queue);
    std::vector<std::thread> threads;
    threads.reserve(n_threads);
    Timer<std::chrono::nanoseconds> t{};

    for (std::size_t thread_idx{}; thread_idx < n_threads; thread_idx++) {
        threads.emplace_back([&vec, thread_idx, n_threads]() {
            for (std::size_t i{thread_idx}; i < n_queue; i += n_threads) {
                auto out_mat = test_mat;
                for (auto cb : mgr::dwt_queue<float>[i]) {
                    out_mat = mgr::dwt_2d_wrapper(
                        out_mat,
                        mgr::lut_bior2_2_f,
                        cb,
                        mgr::padding_mode::symmetric);
                }
                vec[i] = std::move(out_mat);
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
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
