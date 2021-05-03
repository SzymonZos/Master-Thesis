#include "dwt.hpp"
#include "utils.hpp"

namespace mgr {
template<typename T>
dense_array<T> dwt_1d_py(const dense_array<T>& input,
                         const dense_array<T>& filter,
                         mgr::padding_mode mode,
                         std::size_t offset) {
    auto rows = mgr::get_n_dwt_output(static_cast<std::size_t>(input.shape(0)),
                                      static_cast<std::size_t>(filter.size()));
    auto output = make_array<T>(rows, offset);
    mgr::dwt_1d(input.data(),
                static_cast<std::size_t>(input.size()),
                filter.data(),
                static_cast<std::size_t>(filter.size()),
                output.mutable_data(),
                mode,
                offset);
    return output;
}

void init_dwt_1d(py::module_& module) {
    module.def("dwt_1d_f",
               dwt_1d_py<float>,
               R"pbdoc(dwt for float)pbdoc",
               py::arg("input"),
               py::arg("filter"),
               py::arg("mode"),
               py::arg("offset") = 1);

    module.def("dwt_1d_d",
               dwt_1d_py<double>,
               R"pbdoc(dwt for double)pbdoc",
               py::arg("input"),
               py::arg("filter"),
               py::arg("mode"),
               py::arg("offset") = 1);
}
} // namespace mgr