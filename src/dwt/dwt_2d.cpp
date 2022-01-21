#include "dwt_2d.hpp"

#include "dwt.hpp"
#include "dwt_helpers.hpp"

#include <vector>

namespace mgr {

REGISTER_DWT_2D(no_dwt_2d) {
    std::copy_n(input, rows * cols, output);
}
INSTANTIATE(no_dwt_2d, float, float);
INSTANTIATE(no_dwt_2d, double, double);

REGISTER_DWT_2D(dwt_2d_rows) {
    std::size_t out_cols = get_n_dwt_output(cols, n_filter);
    for (std::size_t i{}; i < rows; i++) {
        dwt_1d(input + i * cols,
               cols,
               filter,
               n_filter,
               output + i * out_cols,
               mode);
    }
}
INSTANTIATE(dwt_2d_rows, float, float);
INSTANTIATE(dwt_2d_rows, double, double);

REGISTER_DWT_2D(dwt_2d_cols) {
    for (std::size_t i{}; i < cols; i++) {
        dwt_1d(input + i, rows, filter, n_filter, output + i, mode, cols);
    }
}
INSTANTIATE(dwt_2d_cols, float, float);
INSTANTIATE(dwt_2d_cols, double, double);

REGISTER_DWT_2D(dwt_2d) {
    std::size_t out_cols = get_n_dwt_output(cols, n_filter);
    std::vector<T> tmp;
    tmp.reserve(out_cols * rows);
    dwt_2d_rows(input, rows, cols, filter, n_filter, tmp.data(), mode);
    dwt_2d_cols(tmp.data(), rows, out_cols, filter, n_filter, output, mode);
}
INSTANTIATE(dwt_2d, float, float);
INSTANTIATE(dwt_2d, double, double);

} // namespace mgr
