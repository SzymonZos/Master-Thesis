#ifndef JPEG2000_DWT_HELPERS_HPP
#define JPEG2000_DWT_HELPERS_HPP

#include "dwt_2d.hpp"

namespace mgr {
inline std::size_t get_n_dwt_output(std::size_t n_input,
                                    std::size_t n_filter) {
    if (n_input < 1 || n_filter < 1) {
        return 0;
    }
    return (n_input + n_filter - 1) / 2;
}

using dimensions = std::pair<std::size_t, std::size_t>;

inline dimensions get_out_rows_cols(std::size_t rows_in,
                                    std::size_t cols_in,
                                    std::size_t filter_size,
                                    dwt_mode mode) {
    auto rows_out = get_n_dwt_output(rows_in, filter_size);
    auto cols_out = get_n_dwt_output(cols_in, filter_size);
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

template<typename T>
dimensions get_out_rows_cols(std::size_t rows_in,
                             std::size_t cols_in,
                             std::size_t filter_size,
                             dwt_2d_cb<T, T> cb) {
    auto mode = dwt_cb_map<T>[cb];
    return get_out_rows_cols(rows_in, cols_in, filter_size, mode);
}
} // namespace mgr

#endif // JPEG2000_DWT_HELPERS_HPP
