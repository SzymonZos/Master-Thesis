#include "parallel_for.hpp"

#include <catch.hpp>

namespace mgr {
namespace {
TEST_CASE("Single threaded execution") {
    const std::string expected = "str";

    SECTION("No return from parallel for") {
        std::string res;
        parallel_for(1, 1, [&res, &expected](std::size_t) {
            res.append(expected);
        });
        REQUIRE(res == expected);
    }

    SECTION("Return from parallel for") {
        auto res = parallel_for(1, 1, [&expected](std::size_t) {
            return expected;
        });
        REQUIRE(res.front() == expected);
    }
}
} // namespace
} // namespace mgr
