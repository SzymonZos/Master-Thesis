#include "dwt_2d_helpers.hpp"

namespace mgr {
void init_dwt_2d_f(py::module_& module) {
    init_dwt_2d_generic<float>(module, "floats");
}
} // namespace mgr
