#ifndef JPEG2000_DWT_2D_HPP
#define JPEG2000_DWT_2D_HPP

#include "config.hpp"

#define REGISTER_DWT_2D(name) \
    template<typename T, typename U> \
    void name(const T* restrict input, \
              std::size_t rows, \
              std::size_t cols, \
              const U* restrict filter, \
              std::size_t n_filter, \
              T* restrict output, \
              padding_mode mode)

namespace mgr {
template<typename T, typename U>
using dwt_2d_cb = void (*)(const T* restrict input,
                           std::size_t rows,
                           std::size_t cols,
                           const U* restrict filter,
                           std::size_t n_filter,
                           T* restrict output,
                           padding_mode mode);

REGISTER_DWT_2D(no_dwt_2d);
REGISTER_DWT_2D(dwt_2d_rows);
REGISTER_DWT_2D(dwt_2d_cols);
REGISTER_DWT_2D(dwt_2d);

} // namespace mgr

#endif // JPEG2000_DWT_2D_HPP
