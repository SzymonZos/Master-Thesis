#include "dwt.hpp"
#include <pybind11/pybind11.h>

int add(int i, int j) {
    return i + j;
}

namespace py = pybind11;

PYBIND11_MODULE(jpeg2000_test, m) {
    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------
        .. currentmodule:: cmake_example
        .. autosummary::
           :toctree: _generate
           add
           subtract
    )pbdoc";

    m.def("add", &add, R"pbdoc(
        Add two numbers
        Some other explanation about the add function.
    )pbdoc");

    m.def(
        "subtract",
        [](int i, int j) { return i - j; },
        R"pbdoc(
        Subtract two numbers
        Some other explanation about the subtract function.
    )pbdoc");

    py::enum_<mgr::padding_mode>(m, "padding_mode")
        .value("invalid", mgr::padding_mode::invalid)
        .value("zeropad", mgr::padding_mode::zeropad)
        .value("symmetric", mgr::padding_mode::symmetric)
        .export_values();

    m.def("downsampling_convolution_f",
          &mgr::downsampling_convolution<float, float>,
          R"pbdoc(Something float)pbdoc");

    m.def("downsampling_convolution_d",
          &mgr::downsampling_convolution<double, double>,
          R"pbdoc(Something double)pbdoc");

    m.attr("__version__") = "dev";
}
