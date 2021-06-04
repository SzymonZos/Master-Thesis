#include <array>
#include <iostream>
#include <thread>
#include <vector>

#ifndef __clang__
#include <execution>
#endif

#include "dwt.hpp"
#include "dwt_2d.hpp"
#include "dwt_helpers.hpp"
#include "filter_luts.hpp"
#include "matrix.hpp"
#include "queue.hpp"

#include "Timer.hpp"

namespace {
const mgr::matrix<float> test_mat{{1, 2, 3, 4, 5, 5, 6, 7, 8, 9}, 5, 2};

void demo_filter_luts() {
    for (auto x : mgr::detail::lut_db2<float>) {
        std::cout << x << " ";
    }
    std::cout << "\n\n";
    for (auto x : mgr::lut_db2_f) {
        std::cout << x << " ";
    }
    std::cout << "\n\n";
}

void demo_dwt_1d() {
    std::array<float, 4> in{1, 2, 3, 4};
    std::vector<float> out(
        mgr::get_n_dwt_output(in.size(), mgr::lut_db2_f.size()));
    mgr::dwt_1d(in.data(),
                in.size(),
                mgr::lut_db2_f.data(),
                mgr::lut_db2_f.size(),
                out.data(),
                mgr::padding_mode::symmetric);
    for (auto i : out) {
        std::cout << i << " ";
    }
    std::cout << "\n\n";
}

void demo_dwt_2d() {
    mgr::matrix<float> out_rows{
        test_mat.rows(),
        mgr::get_n_dwt_output(test_mat.cols(), mgr::lut_bior2_2_f.size())};
    mgr::matrix<float> out_cols{
        mgr::get_n_dwt_output(test_mat.rows(), mgr::lut_bior2_2_f.size()),
        test_mat.cols()};
    mgr::matrix<float> out_2d{out_cols.rows(), out_rows.cols()};

    mgr::dwt_2d(test_mat.data(),
                test_mat.rows(),
                test_mat.cols(),
                mgr::lut_bior2_2_f.data(),
                mgr::lut_bior2_2_f.size(),
                out_2d.data(),
                mgr::padding_mode::symmetric);
    std::cout << out_2d << "\n\n";

    mgr::dwt_2d_rows(test_mat.data(),
                     test_mat.rows(),
                     test_mat.cols(),
                     mgr::lut_bior2_2_f.data(),
                     mgr::lut_bior2_2_f.size(),
                     out_rows.data(),
                     mgr::padding_mode::symmetric);
    std::cout << out_rows << "\n\n";

    mgr::dwt_2d_cols(test_mat.data(),
                     test_mat.rows(),
                     test_mat.cols(),
                     mgr::lut_bior2_2_f.data(),
                     mgr::lut_bior2_2_f.size(),
                     out_cols.data(),
                     mgr::padding_mode::symmetric);
    std::cout << out_cols << "\n\n";
}

auto demo_sequential() {
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
auto demo_parallel() {
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

auto demo_my_parallel() {
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

int main() {
    std::cout << "SEQUENTIAL VERSION\n";
    auto vec_seq = demo_sequential();
    for (auto&& mat : vec_seq) {
        std::cout << mat << "\n\n";
    }

#ifndef __clang__
    std::cout << "\n\nPARALLEL VERSION\n";
    auto vec_par = demo_parallel();
    for (auto&& mat : vec_par) {
        std::cout << mat << "\n\n";
    }
#endif

    std::cout << "\n\nMY PARALLEL VERSION\n";
    auto vec_my_par = demo_my_parallel();
    for (auto&& mat : vec_my_par) {
        std::cout << mat << "\n\n";
    }

    std::cout << std::boolalpha << (vec_seq == vec_my_par) << "\n";
    return 0;
}
