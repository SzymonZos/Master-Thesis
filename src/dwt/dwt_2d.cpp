#include "dwt_2d.hpp"
#include "dwt.hpp"

#include <vector>

#define INSTANTIATE_DWT_2D(name, T, U) \
    template void name<T, U>(const T* restrict input, \
                             std::size_t rows, \
                             std::size_t cols, \
                             const U* restrict filter, \
                             std::size_t n_filter, \
                             T* restrict output, \
                             padding_mode mode)

namespace mgr {

REGISTER_DWT_2D(no_dwt_2d) {
    std::copy_n(input, rows * cols, output);
}
INSTANTIATE_DWT_2D(no_dwt_2d, float, float);
INSTANTIATE_DWT_2D(no_dwt_2d, double, double);

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
INSTANTIATE_DWT_2D(dwt_2d_rows, float, float);
INSTANTIATE_DWT_2D(dwt_2d_rows, double, double);

REGISTER_DWT_2D(dwt_2d_cols) {
    for (std::size_t i{}; i < cols; i++) {
        dwt_1d(input + i, rows, filter, n_filter, output + i, mode, cols);
    }
}
INSTANTIATE_DWT_2D(dwt_2d_cols, float, float);
INSTANTIATE_DWT_2D(dwt_2d_cols, double, double);

REGISTER_DWT_2D(dwt_2d) {
    std::size_t out_cols = get_n_dwt_output(cols, n_filter);
    std::vector<T> tmp;
    tmp.reserve(out_cols * rows);
    dwt_2d_rows(input, rows, cols, filter, n_filter, tmp.data(), mode);
    dwt_2d_cols(tmp.data(), rows, out_cols, filter, n_filter, output, mode);
}
INSTANTIATE_DWT_2D(dwt_2d, float, float);
INSTANTIATE_DWT_2D(dwt_2d, double, double);

} // namespace mgr
