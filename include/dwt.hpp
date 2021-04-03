#ifndef JPEG2000_DWT_HPP
#define JPEG2000_DWT_HPP

#include <cstddef>
#define restrict __restrict__

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

template<typename T, typename U>
void downsampling_convolution(const T* restrict input,
                              size_t N,
                              const U* restrict filter,
                              size_t F,
                              T* restrict output,
                              size_t step,
                              padding_mode mode);
} // namespace mgr

#endif // JPEG2000_DWT_HPP
