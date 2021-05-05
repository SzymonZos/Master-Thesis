#include "dwt_2d.hpp"
#include "dwt.hpp"
#include "utils.hpp"

namespace mgr {
template<typename T, dwt_2d_cb<T, T> dwt_cb>
dense_array<T> dwt_2d_py(const dense_array<T>& input,
                         const dense_array<T>& filter,
                         mgr::padding_mode mode) {
    auto rows = get_n_dwt_output(static_cast<std::size_t>(input.shape(0)),
                                 static_cast<std::size_t>(filter.size()));
    auto cols = get_n_dwt_output(static_cast<std::size_t>(input.shape(1)),
                                 static_cast<std::size_t>(filter.size()));
    auto output = make_array<T>(rows, cols);
    dwt_cb(input.data(),
           static_cast<std::size_t>(input.shape(0)),
           static_cast<std::size_t>(input.shape(1)),
           filter.data(),
           static_cast<std::size_t>(filter.size()),
           output.mutable_data(),
           mode);
    return output;
}

void init_dwt_2d(py::module_& module) {
    module.def("dwt_2d_f",
               dwt_2d_py<float, dwt_2d>,
               R"pbdoc(dwt 2d for floats)pbdoc");

    module.def("dwt_2d_rows_f",
               dwt_2d_py<float, dwt_2d_rows>,
               R"pbdoc(dwt 2d rows for floats)pbdoc");

    module.def("dwt_2d_cols_f",
               dwt_2d_py<float, dwt_2d_cols>,
               R"pbdoc(dwt 2d cols for floats)pbdoc");
}
} // namespace mgr
