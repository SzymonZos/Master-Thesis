#include "dwt.hpp"
#include "luts.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

template<typename T>
using dense_array = py::array_t<T, py::array::c_style | py::array::forcecast>;

namespace {
template<typename T>
dense_array<T> make_array(std::size_t rows, std::size_t cols) {
    if (cols == 1) {
        return dense_array<T>{static_cast<py::ssize_t>(rows)};
    }
    return dense_array<T>{{rows, cols}};
}

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
} // namespace

PYBIND11_MODULE(jpeg2000_test, m) {
    m.doc() = R"pbdoc(
        Pybind11 jpeg2000 test plugin
        -----------------------
        .. currentmodule:: jpeg2000_test
        .. autosummary::
           :toctree: _generate
           downsampling_convolution_f
           downsampling_convolution_d
    )pbdoc";

    py::enum_<mgr::padding_mode>(m, "padding_mode")
        .value("invalid", mgr::padding_mode::invalid)
        .value("zeropad", mgr::padding_mode::zeropad)
        .value("symmetric", mgr::padding_mode::symmetric)
        .export_values();

    m.attr("lut_bior2_2_f") = dense_array<float>(mgr::lut_bior2_2_f.size(),
                                                 mgr::lut_bior2_2_f.data());

    m.attr("lut_bior2_2_d") = dense_array<double>(mgr::lut_bior2_2_d.size(),
                                                  mgr::lut_bior2_2_d.data());

    m.def("dwt_1d_f",
          dwt_1d_py<float>,
          R"pbdoc(dwt for float)pbdoc",
          py::arg("input"),
          py::arg("filter"),
          py::arg("mode"),
          py::arg("offset") = 1);

    m.def("dwt_1d_d",
          dwt_1d_py<double>,
          R"pbdoc(dwt for double)pbdoc",
          py::arg("input"),
          py::arg("filter"),
          py::arg("mode"),
          py::arg("offset") = 1);
}
