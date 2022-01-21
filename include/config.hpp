#ifndef JPEG2000_CONFIG_HPP
#define JPEG2000_CONFIG_HPP

#include <cstddef>
#include <cstdint>
#define restrict __restrict__

#define INSTANTIATE(func, ...) template decltype(func<__VA_ARGS__>) func

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
}

#endif // JPEG2000_CONFIG_HPP
