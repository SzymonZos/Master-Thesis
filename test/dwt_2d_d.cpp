#include "dwt_2d_helpers.hpp"

namespace mgr {
void init_dwt_2d_d(py::module_& module) {
    init_dwt_2d_generic<double>(module, "doubles");
}
} // namespace mgr
