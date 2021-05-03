#include "dwt_2d.hpp"
#include "dwt.hpp"
#include "utils.hpp"

namespace mgr {
template<typename T>
dense_array<T> dwt_2d_py(const dense_array<T>& input,
                         const dense_array<T>& filter,
                         mgr::padding_mode mode) {
    auto rows = mgr::get_n_dwt_output(static_cast<std::size_t>(input.shape(0)),
                                      static_cast<std::size_t>(filter.size()));
    auto cols = mgr::get_n_dwt_output(static_cast<std::size_t>(input.shape(1)),
                                      static_cast<std::size_t>(filter.size()));
    auto output = make_array<T>(rows, cols);
    dwt_2d(input.data(),
           static_cast<std::size_t>(input.shape(0)),
           static_cast<std::size_t>(input.shape(1)),
           filter.data(),
           static_cast<std::size_t>(filter.size()),
           output.mutable_data(),
           mode);
    return output;
}

void init_dwt_2d(py::module_& module) {
    module.def("dwt_2d_f", dwt_2d_py<float>, R"pbdoc(dwt 2d for float)pbdoc");
}
} // namespace mgr
