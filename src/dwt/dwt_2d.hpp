#ifndef JPEG2000_DWT_2D_HPP
#define JPEG2000_DWT_2D_HPP

#include "config.hpp"

#include <array>
#include <unordered_map>

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

using dwt_2d_cb_f = dwt_2d_cb<float, float>;
using dwt_2d_cb_d = dwt_2d_cb<double, double>;

REGISTER_DWT_2D(no_dwt_2d);
REGISTER_DWT_2D(dwt_2d_rows);
REGISTER_DWT_2D(dwt_2d_cols);
REGISTER_DWT_2D(dwt_2d);

template<typename T, typename U>
inline constexpr auto lut_dwt_2d_cbs = std::to_array<dwt_2d_cb<T, U>>(
    {no_dwt_2d<T, U>, dwt_2d_rows<T, U>, dwt_2d_cols<T, U>, dwt_2d<T, U>});

enum class dwt_mode { _2d = 0, _rows = 1, _cols = 2, _no = 3 };

template<typename T>
inline std::unordered_map<dwt_2d_cb<T, T>, dwt_mode> dwt_cb_map = {
    {dwt_2d<T, T>, dwt_mode::_2d},
    {dwt_2d_rows<T, T>, dwt_mode::_rows},
    {dwt_2d_cols<T, T>, dwt_mode::_cols},
    {no_dwt_2d<T, T>, dwt_mode::_no}};
} // namespace mgr

#endif // JPEG2000_DWT_2D_HPP
