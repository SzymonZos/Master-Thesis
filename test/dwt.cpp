#include "dwt.hpp"
#include "dwt_helpers.hpp"
#include "utils.hpp"

namespace mgr {
template<typename T>
dense_array<T> dwt_1d_py(const dense_array<T>& input,
                         const dense_array<T>& filter,
                         padding_mode mode) {
    auto rows = get_n_dwt_output(static_cast<std::size_t>(input.shape(0)),
                                 static_cast<std::size_t>(filter.size()));
    auto output = make_array<T>(rows);
    dwt_1d(input.data(),
           static_cast<std::size_t>(input.size()),
           filter.data(),
           static_cast<std::size_t>(filter.size()),
           output.mutable_data(),
           mode);
    return output;
}

void init_dwt_1d(py::module_& module) {
    module.def("dwt_1d_f", dwt_1d_py<float>, R"pbdoc(dwt for float)pbdoc");
    module.def("dwt_1d_d", dwt_1d_py<double>, R"pbdoc(dwt for double)pbdoc");
}
} // namespace mgr
