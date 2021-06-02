#include "utils.hpp"
#include "config.hpp"
#include "filter_luts.hpp"

#include <pybind11/pybind11.h>

PYBIND11_MODULE(jpeg2000_test, m) {
    m.doc() = R"pbdoc(
        Pybind11 jpeg2000 test plugin
        -----------------------
        .. currentmodule:: jpeg2000_test
        .. autosummary::
           :toctree: _generate
           dwt_1d_f
           dwt_1d_d
    )pbdoc";

    py::enum_<mgr::padding_mode>(m, "padding_mode")
        .value("invalid", mgr::padding_mode::invalid)
        .value("zeropad", mgr::padding_mode::zeropad)
        .value("symmetric", mgr::padding_mode::symmetric)
        .export_values();

    m.attr("lut_bior2_2_f") = mgr::dense_array<float>(
        mgr::lut_bior2_2_f.size(),
        mgr::lut_bior2_2_f.data());

    m.attr("lut_bior2_2_d") = mgr::dense_array<double>(
        mgr::lut_bior2_2_d.size(),
        mgr::lut_bior2_2_d.data());

    mgr::init_dwt_1d(m);
    mgr::init_dwt_2d_f(m);
    mgr::init_dwt_2d_d(m);
}
