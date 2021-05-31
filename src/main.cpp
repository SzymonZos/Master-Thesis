#include <algorithm>
#include <array>
#include <iostream>
#include <vector>

#include "dwt.hpp"
#include "dwt_2d.hpp"
#include "dwt_helpers.hpp"
#include "luts.hpp"
#include "matrix.hpp"

namespace mgr {

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

template<typename T, std::size_t N>
mgr::matrix<T> dwt_2d_wrapper(const mgr::matrix<T>& input,
                              const std::array<T, N>& filter,
                              mgr::dwt_2d_cb<T, T> dwt_cb,
                              mgr::padding_mode mode) {
    auto [rows_out, cols_out] = mgr::get_out_rows_cols(input.rows(),
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
