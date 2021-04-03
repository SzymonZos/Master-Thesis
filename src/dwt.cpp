#include "dwt.hpp"

#define INSTANTIATE_CONV(T, U) \
    template void downsampling_convolution<T, U>(const T* restrict input, \
                                                 size_t N, \
                                                 const U* restrict filter, \
                                                 size_t F, \
                                                 T* restrict output, \
                                                 size_t step, \
                                                 padding_mode mode)

namespace mgr {
template<typename T, typename U>
void downsampling_convolution(const T* restrict input,
                              size_t N,
                              const U* restrict filter,
                              size_t F,
                              T* restrict output,
                              size_t step,
                              padding_mode mode) {
    /* This convolution performs efficient downsampling by computing every
     * step'th element of normal convolution (currently tested only for
     * step=1 and step=2).
     */

    size_t i = step - 1, o = 0;

    // left boundary overhang
    for (; i < F && i < N; i += step, ++o) {
        T sum = 0;
        size_t j;
        for (j = 0; j <= i; ++j)
            sum += filter[j] * input[i - j];

        switch (mode) {
        case padding_mode::symmetric:
            while (j < F) {
                size_t k;
                for (k = 0; k < N && j < F; ++j, ++k)
                    sum += filter[j] * input[k];
                for (k = 0; k < N && j < F; ++k, ++j)
                    sum += filter[j] * input[N - 1 - k];
            }
            break;
        case padding_mode::zeropad:
        default:
            break;
        }
        output[o] = sum;
    }

    // center (if input equal or wider than filter: N >= F)
    for (; i < N; i += step, ++o) {
        T sum = 0;
        size_t j;
        for (j = 0; j < F; ++j)
            sum += input[i - j] * filter[j];
        output[o] = sum;
    }

    // center (if filter is wider than input: F > N)
    for (; i < F; i += step, ++o) {
        T sum = 0;
        size_t j = 0;

        switch (mode) {
        case padding_mode::symmetric:
            // Included from original: TODO: j < F-_offset
            /* Iterate over filter in reverse to process elements away from
             * data. This gives a known first input element to process
             * (N-1)
             */
            while (i - j >= N) {
                size_t k;
                for (k = 0; k < N && i - j >= N; ++j, ++k)
                    sum += filter[i - N - j] * input[N - 1 - k];
                for (k = 0; k < N && i - j >= N; ++j, ++k)
                    sum += filter[i - N - j] * input[k];
            }
            break;
        case padding_mode::zeropad:
        default:
            j = i - N + 1;
            break;
        }

        for (; j <= i; ++j)
            sum += filter[j] * input[i - j];

        switch (mode) {
        case padding_mode::symmetric:
            while (j < F) {
                size_t k;
                for (k = 0; k < N && j < F; ++j, ++k)
                    sum += filter[j] * input[k];
                for (k = 0; k < N && j < F; ++k, ++j)
                    sum += filter[j] * input[N - 1 - k];
            }
            break;
        case padding_mode::zeropad:
        default:
            break;
        }
        output[o] = sum;
    }

    // right boundary overhang
    for (; i < N + F - 1; i += step, ++o) {
        T sum = 0;
        size_t j = 0;
        switch (mode) {
        case padding_mode::symmetric:
            // Included from original: TODO: j < F-_offset
            while (i - j >= N) {
                size_t k;
                for (k = 0; k < N && i - j >= N; ++j, ++k)
                    sum += filter[i - N - j] * input[N - 1 - k];
                for (k = 0; k < N && i - j >= N; ++j, ++k)
                    sum += filter[i - N - j] * input[k];
            }
            break;
        case padding_mode::zeropad:
        default:
            j = i - N + 1;
            break;
        }
        for (; j < F; ++j)
            sum += filter[j] * input[i - j];
        output[o] = sum;
    }
}

INSTANTIATE_CONV(float, float);
INSTANTIATE_CONV(double, double);

} // namespace mgr
