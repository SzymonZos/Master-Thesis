#include <cstddef>
#define restrict __restrict__

#include "luts.hpp"

enum MODE {
    MODE_INVALID = -1,
    MODE_ZEROPAD = 0, /* default, signal extended with zeros */
    MODE_SYMMETRIC, /* signal extended symmetrically (mirror)
                     * also known as half-sample symmetric
                     * For extensions greater than signal length,
                     * mirror back and forth:
                     * 2 3 3 2 1 | 1 2 3 | 3 2 1 1 2
                     */
    MODE_CONSTANT_EDGE, /* signal extended with the border value */
    MODE_SMOOTH, /* linear extrapolation (first derivative) */
    MODE_PERIODIC, /* signal is treated as being periodic */
    MODE_PERIODIZATION, /* signal is treated as being periodic, minimal output
                           length */
    MODE_REFLECT, /* signal extended symmetrically (reflect)
                   * also known as whole-sample symmetric
                   * For extensions greater than signal length,
                   * reflect back and forth without repeating edge values:
                   * 1 2 3 2 | 1 2 3 | 2 1 2 3
                   */
    MODE_ANTISYMMETRIC, /* antisymmetric version of "MODE_SYMMETRIC"
                         * also known as half-sample antisymmetric
                         * 2 3 -3 -2 -1 | 1 2 3 | -3 -2 -1 1 2
                         */
    MODE_ANTIREFLECT, /* antisymmetric version of "MODE_REFLECT"
                       * also known as whole-sample antisymmetric
                       * 0 -1 -2 -1 0 | 1 2 3 | 4 5 6 5 4
                       */
    MODE_MAX,
};

template<typename T, typename U>
int downsampling_convolution(const T* restrict input,
                             const size_t N,
                             const U* const restrict filter,
                             const size_t F,
                             T* const restrict output,
                             const size_t step,
                             MODE mode) {
    /* This convolution performs efficient downsampling by computing every
     * step'th element of normal convolution (currently tested only for step=1
     * and step=2).
     */

    size_t i = step - 1, o = 0;

    if (mode == MODE_SMOOTH && N < 2)
        mode = MODE_CONSTANT_EDGE;

    // left boundary overhang
    for (; i < F && i < N; i += step, ++o) {
        T sum = 0;
        size_t j;
        for (j = 0; j <= i; ++j)
            sum += filter[j] * input[i - j];

        switch (mode) {
        case MODE_SYMMETRIC:
            while (j < F) {
                size_t k;
                for (k = 0; k < N && j < F; ++j, ++k)
                    sum += filter[j] * input[k];
                for (k = 0; k < N && j < F; ++k, ++j)
                    sum += filter[j] * input[N - 1 - k];
            }
            break;
        case MODE_ANTISYMMETRIC:
            // half-sample anti-symmetric
            while (j < F) {
                size_t k;
                for (k = 0; k < N && j < F; ++j, ++k)
                    sum -= filter[j] * input[k];
                for (k = 0; k < N && j < F; ++k, ++j)
                    sum += filter[j] * input[N - 1 - k];
            }
            break;
        case MODE_REFLECT:
            while (j < F) {
                size_t k;
                for (k = 1; k < N && j < F; ++j, ++k)
                    sum += filter[j] * input[k];
                for (k = 1; k < N && j < F; ++k, ++j)
                    sum += filter[j] * input[N - 1 - k];
            }
            break;
        case MODE_ANTIREFLECT: {
            // whole-sample anti-symmetric
            size_t k;
            T le = input[0]; // current left edge value
            T tmp = 0;
            while (j < F) {
                for (k = 1; k < N && j < F; ++j, ++k) {
                    tmp = le - (input[k] - input[0]);
                    sum += filter[j] * tmp;
                }
                le = tmp;
                for (k = 1; k < N && j < F; ++j, ++k) {
                    tmp = le + (input[N - 1 - k] - input[N - 1]);
                    sum += filter[j] * tmp;
                }
                le = tmp;
            }
            break;
        }
        case MODE_CONSTANT_EDGE:
            for (; j < F; ++j)
                sum += filter[j] * input[0];
            break;
        case MODE_SMOOTH: {
            size_t k;
            for (k = 1; j < F; ++j, ++k)
                sum += filter[j] * (input[0] + k * (input[0] - input[1]));
            break;
        }
        case MODE_PERIODIC:
            while (j < F) {
                size_t k;
                for (k = 0; k < N && j < F; ++k, ++j)
                    sum += filter[j] * input[N - 1 - k];
            }
            break;
        case MODE_ZEROPAD:
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
        case MODE_SYMMETRIC:
            // Included from original: TODO: j < F-_offset
            /* Iterate over filter in reverse to process elements away from
             * data. This gives a known first input element to process (N-1)
             */
            while (i - j >= N) {
                size_t k;
                for (k = 0; k < N && i - j >= N; ++j, ++k)
                    sum += filter[i - N - j] * input[N - 1 - k];
                for (k = 0; k < N && i - j >= N; ++j, ++k)
                    sum += filter[i - N - j] * input[k];
            }
            break;
        case MODE_ANTISYMMETRIC:
            // half-sample anti-symmetric
            while (i - j >= N) {
                size_t k;
                for (k = 0; k < N && i - j >= N; ++j, ++k)
                    sum -= filter[i - N - j] * input[N - 1 - k];
                for (k = 0; k < N && i - j >= N; ++j, ++k)
                    sum += filter[i - N - j] * input[k];
            }
            break;
        case MODE_REFLECT:
            while (i - j >= N) {
                size_t k;
                for (k = 1; k < N && i - j >= N; ++j, ++k)
                    sum += filter[i - N - j] * input[N - 1 - k];
                for (k = 1; k < N && i - j >= N; ++j, ++k)
                    sum += filter[i - N - j] * input[k];
            }
            break;
        case MODE_ANTIREFLECT: {
            // whole-sample anti-symmetric
            size_t k;
            T re = input[N - 1]; // current right edge value
            T tmp = 0;
            while (i - j >= N) {
                for (k = 1; k < N && i - j >= N; ++j, ++k) {
                    tmp = re - (input[N - 1 - k] - input[N - 1]);
                    sum += filter[i - N - j] * tmp;
                }
                re = tmp;
                for (k = 1; k < N && i - j >= N; ++j, ++k) {
                    tmp = re + (input[k] - input[0]);
                    sum += filter[i - N - j] * tmp;
                }
                re = tmp;
            }
            break;
        }
        case MODE_CONSTANT_EDGE:
            for (; i - j >= N; ++j)
                sum += filter[j] * input[N - 1];
            break;
        case MODE_SMOOTH: {
            size_t k;
            for (k = i - N + 1; i - j >= N; ++j, --k)
                sum += filter[j] *
                       (input[N - 1] + k * (input[N - 1] - input[N - 2]));
            break;
        }
        case MODE_PERIODIC:
            while (i - j >= N) {
                size_t k;
                for (k = 0; k < N && i - j >= N; ++j, ++k)
                    sum += filter[i - N - j] * input[k];
            }
            break;
        case MODE_ZEROPAD:
        default:
            j = i - N + 1;
            break;
        }

        for (; j <= i; ++j)
            sum += filter[j] * input[i - j];

        switch (mode) {
        case MODE_SYMMETRIC:
            while (j < F) {
                size_t k;
                for (k = 0; k < N && j < F; ++j, ++k)
                    sum += filter[j] * input[k];
                for (k = 0; k < N && j < F; ++k, ++j)
                    sum += filter[j] * input[N - 1 - k];
            }
            break;
        case MODE_ANTISYMMETRIC:
            // half-sample anti-symmetric
            while (j < F) {
                size_t k;
                for (k = 0; k < N && j < F; ++j, ++k)
                    sum -= filter[j] * input[k];
                for (k = 0; k < N && j < F; ++k, ++j)
                    sum += filter[j] * input[N - 1 - k];
            }
            break;
        case MODE_REFLECT:
            while (j < F) {
                size_t k;
                for (k = 1; k < N && j < F; ++j, ++k)
                    sum += filter[j] * input[k];
                for (k = 1; k < N && j < F; ++k, ++j)
                    sum += filter[j] * input[N - 1 - k];
            }
            break;
        case MODE_ANTIREFLECT: {
            // whole-sample anti-symmetric
            size_t k;
            T le = input[0]; // current left edge value
            T tmp = 0;
            while (j < F) {
                for (k = 1; k < N && j < F; ++j, ++k) {
                    tmp = le - (input[k] - input[0]);
                    sum += filter[j] * tmp;
                }
                le = tmp;
                for (k = 1; k < N && j < F; ++j, ++k) {
                    tmp = le + (input[N - 1 - k] - input[N - 1]);
                    sum += filter[j] * tmp;
                }
                le = tmp;
            }
            break;
        }
        case MODE_CONSTANT_EDGE:
            for (; j < F; ++j)
                sum += filter[j] * input[0];
            break;
        case MODE_SMOOTH: {
            size_t k;
            for (k = 1; j < F; ++j, ++k)
                sum += filter[j] * (input[0] + k * (input[0] - input[1]));
            break;
        }
        case MODE_PERIODIC:
            while (j < F) {
                size_t k;
                for (k = 0; k < N && j < F; ++k, ++j)
                    sum += filter[j] * input[N - 1 - k];
            }
            break;
        case MODE_ZEROPAD:
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
        case MODE_SYMMETRIC:
            // Included from original: TODO: j < F-_offset
            while (i - j >= N) {
                size_t k;
                for (k = 0; k < N && i - j >= N; ++j, ++k)
                    sum += filter[i - N - j] * input[N - 1 - k];
                for (k = 0; k < N && i - j >= N; ++j, ++k)
                    sum += filter[i - N - j] * input[k];
            }
            break;
        case MODE_ANTISYMMETRIC:
            // half-sample anti-symmetric
            while (i - j >= N) {
                size_t k;
                for (k = 0; k < N && i - j >= N; ++j, ++k)
                    sum -= filter[i - N - j] * input[N - 1 - k];
                for (k = 0; k < N && i - j >= N; ++j, ++k)
                    sum += filter[i - N - j] * input[k];
            }
            break;
        case MODE_REFLECT:
            while (i - j >= N) {
                size_t k;
                for (k = 1; k < N && i - j >= N; ++j, ++k)
                    sum += filter[i - N - j] * input[N - 1 - k];
                for (k = 1; k < N && i - j >= N; ++j, ++k)
                    sum += filter[i - N - j] * input[k];
            }
            break;
        case MODE_ANTIREFLECT: {
            // whole-sample anti-symmetric
            size_t k;
            T re = input[N - 1]; // current right edge value
            T tmp = 0;
            while (i - j >= N) {
                // first reflection
                for (k = 1; k < N && i - j >= N; ++j, ++k) {
                    tmp = re - (input[N - 1 - k] - input[N - 1]);
                    sum += filter[i - N - j] * tmp;
                }
                re = tmp;
                // second reflection
                for (k = 1; k < N && i - j >= N; ++j, ++k) {
                    tmp = re + (input[k] - input[0]);
                    sum += filter[i - N - j] * tmp;
                }
                re = tmp;
            }
            break;
        }
        case MODE_CONSTANT_EDGE:
            for (; i - j >= N; ++j)
                sum += filter[j] * input[N - 1];
            break;
        case MODE_SMOOTH: {
            size_t k;
            for (k = i - N + 1; i - j >= N; ++j, --k)
                sum += filter[j] *
                       (input[N - 1] + k * (input[N - 1] - input[N - 2]));
            break;
        }
        case MODE_PERIODIC:
            while (i - j >= N) {
                size_t k;
                for (k = 0; k < N && i - j >= N; ++j, ++k)
                    sum += filter[i - N - j] * input[k];
            }
            break;
        case MODE_ZEROPAD:
        default:
            j = i - N + 1;
            break;
        }
        for (; j < F; ++j)
            sum += filter[j] * input[i - j];
        output[o] = sum;
    }
    return 0;
}
