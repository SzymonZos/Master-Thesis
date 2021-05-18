#include <array>
#include <functional>
#include <iostream>
#include <utility>

#include "dwt_2d.hpp"
#include "template_helpers.hpp"

namespace mgr {

using cb = int (*)(int);
using dwt_2d_cb_f = dwt_2d_cb<float, float>;

int foo(int c) {
    return c;
}

int foo1(int c) {
    return c;
}

int foo2(int c) {
    return c;
}

int foo3(int c) {
    return c;
}

constexpr std::array<int, 4> lut_dwt = {0, 1, 2, 3};
constexpr std::array<cb, 4> lut_dwt_2 = {foo, foo1, foo2, foo3};
constexpr std::array<dwt_2d_cb_f, 4> lut_dwt_3 = {no_dwt_2d<float, float>,
                                                  dwt_2d_rows<float, float>,
                                                  dwt_2d_cols<float, float>,
                                                  dwt_2d<float, float>};

constexpr int depth = 3;
constexpr std::size_t n_dwt_cbs = lut_dwt.size() - 1;

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
    consteval void fill_queue(mgr::sequence<T, Is...>, Args&&... args) {
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

constexpr auto queue = cartesian_prod<int, 0>{}.calculate(lut_dwt);
constexpr auto queue_2 = cartesian_prod<cb, foo>{}.calculate(lut_dwt_2);
constexpr auto queue_3 = cartesian_prod<dwt_2d_cb_f, no_dwt_2d<float, float>>{}
                             .calculate(lut_dwt_3);

void show_queue() {
    std::cout << get_queue_size() << "\n";
    for (std::size_t i{}; i < get_queue_size(); i++) {
        for (std::size_t j{}; j < depth; j++) {
            std::cout << std::invoke(queue_2[i][j], queue[i][j]) << " ";
        }
        std::cout << "\n";
    }
}
} // namespace mgr
