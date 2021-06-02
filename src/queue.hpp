#ifndef JPEG2000_QUEUE_HPP
#define JPEG2000_QUEUE_HPP

#include "config.hpp"
#include "dwt_2d.hpp"
#include "template_helpers.hpp"

#include <array>

namespace mgr {
namespace detail {

constexpr std::size_t depth = 3;
constexpr std::size_t n_dwt_cbs = lut_dwt_2d_cbs<float, float>.size() - 1;

consteval std::size_t get_queue_size() {
    std::size_t temp{1};
    std::size_t result{temp};
    for (std::size_t i{}; i < depth; i++) {
        temp *= n_dwt_cbs;
        result += temp;
    }
    return result;
}

template<typename Elem, Elem Pad>
struct cartesian_prod {
    template<typename T,
             std::size_t CartesianCounter = depth,
             typename... Args>
    consteval auto calculate(T&& array, Args&&... args) {
        for (std::size_t i{}; i < array.size(); i++) {
            if constexpr (CartesianCounter != 0) {
                if (i) {
                    calculate<T, CartesianCounter - 1>(
                        array,
                        std::forward<Args>(args)...,
                        array[i]);
                } else {
                    fill_queue(pad_sequence<Elem, Pad, CartesianCounter>{},
                               std::forward<Args>(args)...);
                }
            } else {
                fill_queue(std::forward<Args>(args)...);
                break;
            }
        }
        return queue_;
    }

private:
    template<typename... Args, typename T, T... Is>
    consteval void fill_queue(sequence<T, Is...>, Args&&... args) {
        queue_[queue_counter_++] = std::to_array(
            {std::forward<Args>(args)..., Is...});
    }

    template<typename... Args>
    consteval void fill_queue(Args&&... args) {
        queue_[queue_counter_++] = std::to_array(
            {std::forward<Args>(args)...});
    }

    std::array<std::array<Elem, depth>, get_queue_size()> queue_{};
    std::size_t queue_counter_{};
};
} // namespace detail

template<typename T>
inline constexpr auto
    dwt_queue = detail::cartesian_prod<dwt_2d_cb<T, T>, no_dwt_2d<T, T>>{}
                    .calculate(lut_dwt_2d_cbs<T, T>);

} // namespace mgr

#endif // JPEG2000_QUEUE_HPP
