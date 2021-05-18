#ifndef JPEG2000_TEMPLATE_HELPERS_HPP
#define JPEG2000_TEMPLATE_HELPERS_HPP

namespace mgr {

template<typename T, T... Seq>
struct sequence;

template<typename T, T N>
using int_const = std::integral_constant<T, N>;

namespace detail {
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
} // namespace detail

template<typename T, T... Seq>
struct sequence {
    using value_type = T;
    static constexpr std::size_t size() noexcept {
        return sizeof...(Seq);
    }
};

template<std::size_t... Indices>
using index_sequence = sequence<std::size_t, Indices...>;

template<typename T, T N>
using make_integer_sequence = detail::log_make_sequence_t<T, int_const<T, N>>;

template<std::size_t N>
using make_index_sequence = make_integer_sequence<std::size_t, N>;

template<typename T, T Pad, std::size_t... Is>
constexpr auto pad_sequence_impl(const mgr::index_sequence<Is...>&)
    -> decltype(mgr::sequence<T, (Is, Pad)...>{});

template<typename T, T Pad, std::size_t N>
using pad_sequence = decltype(pad_sequence_impl<T, Pad>(
    std::declval<mgr::make_index_sequence<N>>()));
} // namespace mgr

#endif // JPEG2000_TEMPLATE_HELPERS_HPP
