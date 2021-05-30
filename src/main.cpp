#include <algorithm>
#include <array>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <vector>

#include "dwt.hpp"
#include "dwt_2d.hpp"
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
void show_queue();

using dwt_2d_cb_f = dwt_2d_cb<float, float>;
constexpr int depth = 3;
constexpr std::size_t n_dwt_cbs = 3;

consteval std::size_t get_queue_size() {
    std::size_t temp{1};
    std::size_t result{temp};
    for (std::size_t i{}; i < depth; i++) {
        temp *= n_dwt_cbs;
        result += temp;
    }
    return result;
}

std::array<dwt_2d_cb_f, depth> get_queue();
const std::array<std::array<dwt_2d_cb_f, depth>, get_queue_size()>&
get_queue_all();
} // namespace mgr

enum class dwt_mode { _2d = 0, _rows = 1, _cols = 2, _no = 3 };

template<typename T>
std::unordered_map<mgr::dwt_2d_cb<T, T>, dwt_mode> dwt_cb_map = {
    {mgr::dwt_2d<T, T>, dwt_mode::_2d},
    {mgr::dwt_2d_rows<T, T>, dwt_mode::_rows},
    {mgr::dwt_2d_cols<T, T>, dwt_mode::_cols},
    {mgr::no_dwt_2d<T, T>, dwt_mode::_no}};

template<typename T>
std::pair<std::size_t, std::size_t>
get_out_rows_cols(std::size_t rows_in,
                  std::size_t cols_in,
                  std::size_t filter_size,
                  mgr::dwt_2d_cb<T, T> cb) {
    auto rows_out = mgr::get_n_dwt_output(rows_in, filter_size);
    auto cols_out = mgr::get_n_dwt_output(cols_in, filter_size);
    auto mode = dwt_cb_map<T>[cb];
    switch (mode) {
    case dwt_mode::_2d:
        return {rows_out, cols_out};
    case dwt_mode::_rows:
        return {rows_in, cols_out};
    case dwt_mode::_cols:
        return {rows_out, cols_in};
    case dwt_mode::_no:
        return {rows_in, cols_in};
    default:
        return {};
    }
}

template<typename T, std::size_t N>
mgr::matrix<T> dwt_2d_wrapper(const mgr::matrix<T>& input,
                              const std::array<T, N>& filter,
                              mgr::dwt_2d_cb<T, T> dwt_cb,
                              mgr::padding_mode mode) {
    auto [rows_out, cols_out] = get_out_rows_cols(input.rows(),
                                                  input.cols(),
                                                  filter.size(),
                                                  dwt_cb);
    mgr::matrix<T> output(rows_out, cols_out);
    dwt_cb(input.data(),
           input.rows(),
           input.cols(),
           filter.data(),
           filter.size(),
           output.data(),
           mode);
    return output;
}

int main() {
    //    mgr::threadTesting();
    for (auto x : mgr::detail::lut_db2<float>) {
        std::cout << x << " ";
    }
    std::cout << "\n\n";
    for (auto x : mgr::lut_db2_f) {
        std::cout << x << " ";
    }
    std::cout << "\n\n";

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

    mgr::matrix<float> test_mat{{1, 2, 3, 4, 5, 5, 6, 7, 8, 9}, 5, 2};
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

    auto queue = mgr::get_queue();
    auto out_mat = out_2d;
    for (auto cb : queue) {
        out_mat = dwt_2d_wrapper(out_mat,
                                 mgr::lut_bior2_2_f,
                                 cb,
                                 mgr::padding_mode::symmetric);
        std::cout << out_mat << "\n\n";
    }
    const auto& all_q = mgr::get_queue_all();
    for (const auto& cbs : all_q) {
        auto out_mat_2 = test_mat;
        for (auto cb : cbs) {
            out_mat_2 = dwt_2d_wrapper(out_mat_2,
                                       mgr::lut_bior2_2_f,
                                       cb,
                                       mgr::padding_mode::symmetric);
        }
        std::cout << out_mat_2 << "\n\n";
    }
    mgr::show_queue();
    return 0;
}
