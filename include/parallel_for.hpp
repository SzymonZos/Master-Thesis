#ifndef JPEG2000_PARALLEL_FOR_HPP
#define JPEG2000_PARALLEL_FOR_HPP

#include "config.hpp"

#include <functional>
#include <thread>
#include <vector>

#include <iostream>

namespace mgr {
namespace detail {

template<typename Func>
void parallel_for(std::size_t n_threads, Func&& func) {
    std::vector<std::thread> threads;
    threads.reserve(n_threads);

    for (std::size_t thread_idx{}; thread_idx < n_threads; thread_idx++) {
        threads.emplace_back(std::forward<Func>(func), thread_idx);
    }
    for (auto& thread : threads) {
        thread.join();
    }
}
} // namespace detail

/*
 * template <typename T>
auto capture_example(T&& value) {
  struct { T value; } cap{std::forward<T>(value)};
  return [cap = std::move(cap)]() {  use cap.value ; };
};
 */

template<typename Func>
void parallel_for(std::size_t n_threads, std::size_t n_elements, Func&& func) {
    struct {
        Func func;
    } cap{std::forward<Func>(func)};
    detail::parallel_for(n_threads,
                         [n_threads, n_elements, cap = std::move(cap)](
                             std::size_t thread_idx) mutable {
                             for (std::size_t i{thread_idx}; i < n_elements;
                                  i += n_threads) {
                                 //                std::cout << i << "\n";
                                 std::invoke(std::forward<Func>(cap.func), i);
                             }
                         });
}
} // namespace mgr

#endif // JPEG2000_PARALLEL_FOR_HPP
