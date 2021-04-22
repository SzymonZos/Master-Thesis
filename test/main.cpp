#include "dwt.hpp"
#include "luts.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

template<typename T>
using dense_array = py::array_t<T, py::array::c_style | py::array::forcecast>;

template<typename T>
dense_array<T> make_array(py::ssize_t rows, py::ssize_t cols) {
    if (cols == 1) {
        return dense_array<T>{rows};
    }
    return dense_array<T>{{rows, cols}};
}

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

    m.def(
        "downsampling_convolution_f",
        [](dense_array<float> input,
           dense_array<float> filter,
           mgr::padding_mode mode,
           std::size_t offset) {
            auto output = make_array<float>(
                mgr::get_n_dwt_output(input.shape(0), filter.size()),
                offset);
            mgr::downsampling_convolution(input.data(),
                                          input.size(),
                                          filter.data(),
                                          filter.size(),
                                          output.mutable_data(),
                                          mode,
                                          offset);
            return output;
        },
        R"pbdoc(dwt for floats)pbdoc",
        py::arg("input"),
        py::arg("filter"),
        py::arg("mode"),
        py::arg("offset") = 1);

    m.def("downsampling_convolution_d",
          &mgr::downsampling_convolution<double, double>,
          R"pbdoc(dwt for doubles)pbdoc");
}
