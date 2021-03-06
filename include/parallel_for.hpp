#ifndef JPEG2000_PARALLEL_FOR_HPP
#define JPEG2000_PARALLEL_FOR_HPP

#include "config.hpp"

#include <concepts>
#include <functional>
#include <thread>
#include <vector>

namespace mgr {
namespace detail {

// clang-format off
template<typename Func, typename... Args>
// std::invocable<Func, Args...> should really be used in conjunction
// enable it with the release of libc++13 (support of <concepts> header)
concept no_returnable = std::same_as<std::invoke_result_t<Func, Args...>, void>;

template<typename Func, typename... Args>
concept returnable = !no_returnable<Func, Args...>;
// clang-format on

template<typename Func>
void parallel_for(std::size_t n_threads, const Func& func) {
    std::vector<std::thread> threads;
    threads.reserve(n_threads);
    for (std::size_t thread_idx{}; thread_idx < n_threads; thread_idx++) {
        threads.emplace_back(func, thread_idx);
    }
    for (auto& thread : threads) {
        thread.join();
    }
}
} // namespace detail

template<typename Func>
requires detail::no_returnable<Func, std::size_t>
void parallel_for(std::size_t n_threads, std::size_t n_elements, Func&& func) {
    detail::parallel_for(
        n_threads,
        [n_threads, n_elements, func = std::forward<Func>(func)](
            std::size_t thread_idx) mutable {
            for (std::size_t i{thread_idx}; i < n_elements; i += n_threads) {
                func(i);
            }
        });
}

template<typename Func>
requires detail::returnable<Func, std::size_t>
auto parallel_for(std::size_t n_threads, std::size_t n_elements, Func&& func) {
    std::vector<std::invoke_result_t<Func, std::size_t>> result(n_elements);
    detail::parallel_for(
        n_threads,
        [n_threads, n_elements, func = std::forward<Func>(func), &result](
            std::size_t thread_idx) mutable {
            for (std::size_t i{thread_idx}; i < n_elements; i += n_threads) {
                result[i] = func(i);
            }
        });
    return result;
}
} // namespace mgr

#endif // JPEG2000_PARALLEL_FOR_HPP
