#ifndef JPEG2000_ENTROPY_HPP
#define JPEG2000_ENTROPY_HPP

#include "config.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <unordered_map>

namespace mgr {
namespace detail {
template<typename T>
T log2(T x) {
    if constexpr (std::is_same_v<float, std::remove_cvref<T>>) {
        return std::log2f(x);
    } else {
        return std::log2(x);
    }
}
} // namespace detail

template<typename T, typename U>
T memoryless_entropy(const U* restrict arr, std::size_t len) {
    std::unordered_map<U, std::size_t> hist;
    hist.reserve(256);
    for (std::size_t i{}; i < len; i++) {
        hist[arr[i]]++;
    }
    T entropy = std::accumulate(
        hist.begin(),
        hist.end(),
        T{},
        [len](T prev, auto&& curr) {
            T tmp = static_cast<T>(curr.second) / static_cast<T>(len);
            return prev + tmp * detail::log2(T{1} / tmp);
        });
    return entropy / static_cast<T>(len);
}
} // namespace mgr

#endif // JPEG2000_ENTROPY_HPP
