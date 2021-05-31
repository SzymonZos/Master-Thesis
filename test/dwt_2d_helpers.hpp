#ifndef JPEG2000_TEST_DWT_2D_HELPERS_HPP
#define JPEG2000_TEST_DWT_2D_HELPERS_HPP

#include "dwt.hpp"
#include "dwt_2d.hpp"
#include "dwt_helpers.hpp"
#include "utils.hpp"

#include <string_view>

namespace mgr {

template<typename T, dwt_2d_cb<T, T> dwt_cb, dwt_mode dwt_mode>
dense_array<T> dwt_2d_py(const dense_array<T>& input,
                         const dense_array<T>& filter,
                         mgr::padding_mode mode) {
    auto rows_in = static_cast<std::size_t>(input.shape(0));
    auto cols_in = static_cast<std::size_t>(input.shape(1));
    auto filter_size = static_cast<std::size_t>(filter.size());
    auto [rows_out, cols_out] = get_out_rows_cols(rows_in,
                                                  cols_in,
                                                  filter_size,
                                                  dwt_mode);
    auto output = make_array<T>(rows_out, cols_out);

    dwt_cb(input.data(),
           rows_in,
           cols_in,
           filter.data(),
           filter_size,
           output.mutable_data(),
           mode);
    return output;
}

template<typename T>
void init_dwt_2d_generic(py::module_& module, std::string_view name) {
    using namespace std::string_literals;
    module.def(("dwt_2d_"s += name.front()).c_str(),
               dwt_2d_py<T, dwt_2d, dwt_mode::_2d>,
               ("dwt 2d for"s += name).c_str());

    module.def(("dwt_2d_rows_"s += name.front()).c_str(),
               dwt_2d_py<T, dwt_2d_rows, dwt_mode::_rows>,
               ("dwt 2d rows for "s += name).c_str());

    module.def(("dwt_2d_cols_"s += name.front()).c_str(),
               dwt_2d_py<T, dwt_2d_cols, dwt_mode::_cols>,
               ("dwt 2d cols for "s += name).c_str());
}
} // namespace mgr

#endif // JPEG2000_TEST_DWT_2D_HELPERS_HPP
