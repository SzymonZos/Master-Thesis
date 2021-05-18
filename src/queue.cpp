#include <array>
#include <functional>
#include <iostream>
#include <utility>

namespace mgr {
template<typename T, T N>
using int_const = std::integral_constant<T, N>;

template<typename T, T... Seq>
struct sequence {
    using value_type = T;
    static constexpr std::size_t size() noexcept {
        return sizeof...(Seq);
    }
};

template<std::size_t... Indices>
using index_sequence = sequence<std::size_t, Indices...>;

template<typename Offset, typename Lhs, typename Rhs>
struct merge;

template<typename T, T Offset, T... Lhs, T... Rhs>
struct merge<int_const<T, Offset>, sequence<T, Lhs...>, sequence<T, Rhs...>> {
    using type = sequence<T, Lhs..., (Offset + Rhs)...>;
};

template<typename Offset, typename Lhs, typename Rhs>
using merge_t = typename merge<Offset, Lhs, Rhs>::type;

template<typename T, typename N>
struct log_make_sequence;

template<typename T, typename N>
using log_make_sequence_t = typename log_make_sequence<T, N>::type;

template<typename T, typename N>
struct log_make_sequence {
    using L = int_const<T, N::value / 2>;
    using R = int_const<T, N::value - L::value>;
    using type = merge_t<L,
                         log_make_sequence_t<T, L>,
                         log_make_sequence_t<T, R>>;
};

template<typename T>
struct log_make_sequence<T, int_const<T, 0>> {
    using type = sequence<T>;
};

template<typename T>
struct log_make_sequence<T, int_const<T, 1>> {
    using type = sequence<T, 0>;
};

template<typename T, T N>
using make_integer_sequence = log_make_sequence_t<T, int_const<T, N>>;

template<std::size_t N>
using make_index_sequence = make_integer_sequence<std::size_t, N>;
} // namespace mgr

namespace mgr {
template<typename T, T Pad, std::size_t... Is>
constexpr auto pad_sequence_impl(const mgr::index_sequence<Is...>&)
    -> decltype(mgr::sequence<T, (Is, Pad)...>{});

template<typename T, T Pad, std::size_t N>
using pad_sequence = decltype(pad_sequence_impl<T, Pad>(
    std::declval<mgr::make_index_sequence<N>>()));

using cb = int (*)(int);

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
