#ifndef JPEG2000_DWT_HPP
#define JPEG2000_DWT_HPP

#include "config.hpp"

namespace mgr {

std::size_t get_n_dwt_output(std::size_t n_input, std::size_t n_filter);

template<typename T, typename U>
void downsampling_convolution(const T* restrict input,
                              size_t n_input,
                              const U* restrict filter,
                              size_t n_filter,
                              T* restrict output,
                              padding_mode mode,
                              std::size_t offset = 1);
} // namespace mgr

#endif // JPEG2000_DWT_HPP
