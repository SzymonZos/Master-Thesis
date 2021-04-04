#include "dwt.hpp"

#define INSTANTIATE_CONV(T, U) \
    template void downsampling_convolution<T, U>(const T* restrict input, \
                                                 size_t n_input, \
                                                 const U* restrict filter, \
                                                 size_t n_filter, \
                                                 T* restrict output, \
                                                 padding_mode mode)

namespace mgr {
std::size_t get_n_dwt_output(std::size_t n_input, std::size_t n_filter) {
    if (n_input < 1 || n_filter < 1) {
        return 0;
    }
    return (n_input + n_filter - 1) / 2;
}

template<typename T, typename U>
void downsampling_convolution(const T* restrict input,
                              size_t n_input,
                              const U* restrict filter,
                              size_t n_filter,
                              T* restrict output,
                              padding_mode mode) {
    std::size_t step = 2;
    std::size_t i = step - 1, o = 0;

    // left boundary overhang
    for (; i < n_filter && i < n_input; i += step, ++o) {
        T sum = 0;
        size_t j;
        for (j = 0; j <= i; ++j)
            sum += filter[j] * input[i - j];

        switch (mode) {
        case padding_mode::symmetric:
            while (j < n_filter) {
                size_t k;
                for (k = 0; k < n_input && j < n_filter; ++j, ++k)
                    sum += filter[j] * input[k];
                for (k = 0; k < n_input && j < n_filter; ++k, ++j)
                    sum += filter[j] * input[n_input - 1 - k];
            }
            break;
        case padding_mode::zeropad:
        default:
            break;
        }
        output[o] = sum;
    }

    // center (if input equal or wider than filter: N >= F)
    for (; i < n_input; i += step, ++o) {
        T sum = 0;
        size_t j;
        for (j = 0; j < n_filter; ++j)
            sum += input[i - j] * filter[j];
        output[o] = sum;
    }

    // center (if filter is wider than input: F > N)
    for (; i < n_filter; i += step, ++o) {
        T sum = 0;
        size_t j = 0;

        switch (mode) {
        case padding_mode::symmetric:
            // Included from original: TODO: j < F-_offset
            /* Iterate over filter in reverse to process elements away from
             * data. This gives a known first input element to process
             * (N-1)
             */
            while (i - j >= n_input) {
                size_t k;
                for (k = 0; k < n_input && i - j >= n_input; ++j, ++k)
                    sum += filter[i - n_input - j] * input[n_input - 1 - k];
                for (k = 0; k < n_input && i - j >= n_input; ++j, ++k)
                    sum += filter[i - n_input - j] * input[k];
            }
            break;
        case padding_mode::zeropad:
        default:
            j = i - n_input + 1;
            break;
        }

        for (; j <= i; ++j)
            sum += filter[j] * input[i - j];

        switch (mode) {
        case padding_mode::symmetric:
            while (j < n_filter) {
                size_t k;
                for (k = 0; k < n_input && j < n_filter; ++j, ++k)
                    sum += filter[j] * input[k];
                for (k = 0; k < n_input && j < n_filter; ++k, ++j)
                    sum += filter[j] * input[n_input - 1 - k];
            }
            break;
        case padding_mode::zeropad:
        default:
            break;
        }
        output[o] = sum;
    }

    // right boundary overhang
    for (; i < n_input + n_filter - 1; i += step, ++o) {
        T sum = 0;
        size_t j = 0;
        switch (mode) {
        case padding_mode::symmetric:
            // Included from original: TODO: j < F-_offset
            while (i - j >= n_input) {
                size_t k;
                for (k = 0; k < n_input && i - j >= n_input; ++j, ++k)
                    sum += filter[i - n_input - j] * input[n_input - 1 - k];
                for (k = 0; k < n_input && i - j >= n_input; ++j, ++k)
                    sum += filter[i - n_input - j] * input[k];
            }
            break;
        case padding_mode::zeropad:
        default:
            j = i - n_input + 1;
            break;
        }
        for (; j < n_filter; ++j)
            sum += filter[j] * input[i - j];
        output[o] = sum;
    }
}

INSTANTIATE_CONV(float, float);
INSTANTIATE_CONV(double, double);

} // namespace mgr
