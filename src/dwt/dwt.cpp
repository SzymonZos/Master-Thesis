#include "dwt.hpp"

#define INSTANTIATE_CONV(T, U) \
    template void dwt_1d<T, U>(const T* restrict input, \
                               size_t n_input, \
                               const U* restrict filter, \
                               size_t n_filter, \
                               T* restrict output, \
                               padding_mode mode, \
                               std::size_t offset)

namespace mgr {
std::size_t get_n_dwt_output(std::size_t n_input, std::size_t n_filter) {
    if (n_input < 1 || n_filter < 1) {
        return 0;
    }
    return (n_input + n_filter - 1) / 2;
}

template<typename T, typename U>
void dwt_1d(const T* restrict input,
            size_t n_input,
            const U* restrict filter,
            size_t n_filter,
            T* restrict output,
            padding_mode mode,
            std::size_t offset) {
    constexpr std::size_t step = 2;
    std::size_t i = step - 1;
    std::size_t out_idx = 0;

    // left boundary overhang
    for (; i < n_filter && i < n_input; i += step, out_idx += offset) {
        T sum = 0;
        std::size_t j = 0;
        for (; j <= i; ++j) {
            sum += filter[j] * input[(i - j) * offset];
        }
        switch (mode) {
        case padding_mode::symmetric:
            while (j < n_filter) {
                std::size_t k = 0;
                for (; k < n_input && j < n_filter; ++j, ++k) {
                    sum += filter[j] * input[k * offset];
                }
                for (k = 0; k < n_input && j < n_filter; ++k, ++j) {
                    sum += filter[j] * input[(n_input - 1 - k) * offset];
                }
            }
            break;
        case padding_mode::zeropad:
        default:
            break;
        }
        output[out_idx] = sum;
    }

    // center (if input equal or wider than filter: N >= F)
    for (; i < n_input; i += step, out_idx += offset) {
        T sum = 0;
        for (std::size_t j = 0; j < n_filter; ++j) {
            sum += input[(i - j) * offset] * filter[j];
        }
        output[out_idx] = sum;
    }

    // center (if filter is wider than input: F > N)
    for (; i < n_filter; i += step, out_idx += offset) {
        T sum = 0;
        std::size_t j = 0;
        switch (mode) {
        case padding_mode::symmetric:
            /* Iterate over filter in reverse to process elements away from
             * data. This gives a known first input element to process (N-1)
             */
            while (i - j >= n_input) {
                std::size_t k = 0;
                for (; k < n_input && i - j >= n_input; ++j, ++k) {
                    sum += filter[i - n_input - j] *
                           input[(n_input - 1 - k) * offset];
                }
                for (k = 0; k < n_input && i - j >= n_input; ++j, ++k) {
                    sum += filter[i - n_input - j] * input[k * offset];
                }
            }
            break;
        case padding_mode::zeropad:
        default:
            j = i - n_input + 1;
            break;
        }

        for (; j <= i; ++j) {
            sum += filter[j] * input[(i - j) * offset];
        }

        switch (mode) {
        case padding_mode::symmetric:
            while (j < n_filter) {
                std::size_t k = 0;
                for (; k < n_input && j < n_filter; ++j, ++k) {
                    sum += filter[j] * input[k * offset];
                }
                for (k = 0; k < n_input && j < n_filter; ++k, ++j) {
                    sum += filter[j] * input[(n_input - 1 - k) * offset];
                }
            }
            break;
        case padding_mode::zeropad:
        default:
            break;
        }
        output[out_idx] = sum;
    }

    // right boundary overhang
    for (; i < n_input + n_filter - 1; i += step, out_idx += offset) {
        T sum = 0;
        size_t j = 0;
        switch (mode) {
        case padding_mode::symmetric:
            while (i - j >= n_input) {
                std::size_t k = 0;
                for (; k < n_input && i - j >= n_input; ++j, ++k) {
                    sum += filter[i - n_input - j] *
                           input[(n_input - 1 - k) * offset];
                }
                for (k = 0; k < n_input && i - j >= n_input; ++j, ++k) {
                    sum += filter[i - n_input - j] * input[k * offset];
                }
            }
            break;
        case padding_mode::zeropad:
        default:
            j = i - n_input + 1;
            break;
        }
        for (; j < n_filter; ++j) {
            sum += filter[j] * input[(i - j) * offset];
        }
        output[out_idx] = sum;
    }
}

INSTANTIATE_CONV(float, float);
INSTANTIATE_CONV(double, double);

} // namespace mgr
