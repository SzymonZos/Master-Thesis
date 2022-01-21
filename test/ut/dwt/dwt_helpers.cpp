#include "dwt_helpers.hpp"

#include <catch.hpp>

namespace mgr {
namespace {
TEST_CASE("get_n_dwt_output() zero input, zero filter", "[dwt]") {
    auto result = get_n_dwt_output(0, 0);
    REQUIRE(result == 0);
}
} // namespace
} // namespace mgr
