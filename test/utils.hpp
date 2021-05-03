#ifndef JPEG2000_TEST_UTILS_HPP
#define JPEG2000_TEST_UTILS_HPP

#include <pybind11/numpy.h>

namespace py = pybind11;

namespace mgr {
template<typename T>
using dense_array = py::array_t<T, py::array::c_style | py::array::forcecast>;

template<typename T>
dense_array<T> make_array(std::size_t rows, std::size_t cols) {
    if (cols == 1) {
        return dense_array<T>{static_cast<py::ssize_t>(rows)};
    }
    return dense_array<T>{{rows, cols}};
}

void init_dwt_1d(py::module_& module);
void init_dwt_2d(py::module_& module);
} // namespace mgr

#endif // JPEG2000_TEST_UTILS_HPP
