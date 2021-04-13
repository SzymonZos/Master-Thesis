#ifndef JPEG2000_DWT_HPP
#define JPEG2000_DWT_HPP

#include "config.hpp"

namespace mgr {
enum class padding_mode {
    invalid = -1,
    zeropad = 0, // default, signal extended with zeros
    symmetric, /* signal extended symmetrically (mirror)
                * also known as half-sample symmetric
                * For extensions greater than signal length,
                * mirror back and forth:
                * 2 3 3 2 1 | 1 2 3 | 3 2 1 1 2
                */
};

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
