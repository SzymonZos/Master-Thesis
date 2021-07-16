#ifndef JPEG2000_FILTER_LUTS_HPP
#define JPEG2000_FILTER_LUTS_HPP

#include <algorithm>
#include <array>
#include <type_traits>

#define INSTANTIATE_LUT(lut) \
    inline constexpr auto lut##_f = detail::reverse_lut(detail::lut<float>); \
    inline constexpr auto lut##_d = detail::reverse_lut(detail::lut<double>)

namespace mgr {
namespace detail {

template<typename T, std::size_t N>
constexpr auto reverse_lut(const std::array<T, N>& lut) {
    std::array<T, N> reversed;
    std::reverse_copy(lut.begin(), lut.end(), reversed.begin());
    return reversed;
}

template<typename T>
inline constexpr auto lut_db1 = std::to_array<T>(
    {7.071067811865475244008443621048490392848359376884740365883398e-01,
     7.071067811865475244008443621048490392848359376884740365883398e-01});

template<typename T>
inline constexpr auto lut_db2 = std::to_array<T>(
    {4.829629131445341433748715998644486838169524195042022752011715e-01,
     8.365163037378079055752937809168732034593703883484392934953414e-01,
     2.241438680420133810259727622404003554678835181842717613871683e-01,
     -1.294095225512603811744494188120241641745344506599652569070016e-01});

template<typename T>
inline constexpr auto lut_db3 = std::to_array<T>(
    {3.326705529500826159985115891390056300129233992450683597084705e-01,
     8.068915093110925764944936040887134905192973949948236181650920e-01,
     4.598775021184915700951519421476167208081101774314923066433867e-01,
     -1.350110200102545886963899066993744805622198452237811919756862e-01,
     -8.544127388202666169281916918177331153619763898808662976351748e-02,
     3.522629188570953660274066471551002932775838791743161039893406e-02});

template<typename T>
inline constexpr auto
    lut_bior2_2 = std::to_array<T>({-0.1767766952966368811002110905262,
                                    0.3535533905932737622004221810524,
                                    1.0606601717798212866012665431573,
                                    0.3535533905932737622004221810524,
                                    -0.1767766952966368811002110905262,
                                    0.0});

template<typename T>
inline constexpr auto
    lut_bior2_2_h = std::to_array<T>({0.,
                                      0.3535533905932737622004221810524,
                                      0.7071067811865475244008443621048,
                                      0.3535533905932737622004221810524,
                                      0.,
                                      0.});

} // namespace detail

INSTANTIATE_LUT(lut_db1);
INSTANTIATE_LUT(lut_db2);
INSTANTIATE_LUT(lut_db3);
INSTANTIATE_LUT(lut_bior2_2);
INSTANTIATE_LUT(lut_bior2_2_h);
} // namespace mgr

#endif // JPEG2000_FILTER_LUTS_HPP
