#ifndef JPEG2000_DWT_HPP
#define JPEG2000_DWT_HPP

#include "config.hpp"

namespace mgr {

template<typename T, typename U>
void dwt_1d(const T* restrict input,
            std::size_t n_input,
            const U* restrict filter,
            std::size_t n_filter,
            T* restrict output,
            padding_mode mode,
            std::size_t offset = 1);
} // namespace mgr

#endif // JPEG2000_DWT_HPP
