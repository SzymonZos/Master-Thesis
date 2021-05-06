#include "dwt_2d.hpp"
#include "dwt.hpp"
#include "utils.hpp"

namespace mgr {
enum class dwt_mode { _2d = 0, _rows = 1, _cols = 2 };

std::pair<std::size_t, std::size_t> get_out_rows_cols(std::size_t rows_in,
                                                      std::size_t cols_in,
                                                      std::size_t filter_size,
                                                      dwt_mode mode) {
    auto rows_out = get_n_dwt_output(rows_in, filter_size);
    auto cols_out = get_n_dwt_output(cols_in, filter_size);
    switch (mode) {
    case dwt_mode::_2d:
        return {rows_out, cols_out};
    case dwt_mode::_rows:
        return {rows_in, cols_out};
    case dwt_mode::_cols:
        return {rows_out, cols_in};
    default:
        return {};
    }
}

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
           static_cast<std::size_t>(filter.size()),
           output.mutable_data(),
           mode);
    return output;
}

void init_dwt_2d(py::module_& module) {
    module.def("dwt_2d_f",
               dwt_2d_py<float, dwt_2d, dwt_mode::_2d>,
               R"pbdoc(dwt 2d for floats)pbdoc");

    module.def("dwt_2d_rows_f",
               dwt_2d_py<float, dwt_2d_rows, dwt_mode::_rows>,
               R"pbdoc(dwt 2d rows for floats)pbdoc");

    module.def("dwt_2d_cols_f",
               dwt_2d_py<float, dwt_2d_cols, dwt_mode::_cols>,
               R"pbdoc(dwt 2d cols for floats)pbdoc");
}
} // namespace mgr
