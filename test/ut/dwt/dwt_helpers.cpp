#include "dwt_helpers.hpp"

#include <catch.hpp>

namespace mgr {
namespace {
TEST_CASE("get_n_dwt_output(): zero input, zero filter", "[dwt][utils]") {
    auto result = get_n_dwt_output(0, 0);
    REQUIRE(result == 0);
}

TEST_CASE("get_n_dwt_output(): zero input, non-zero filter", "[dwt][utils]") {
    auto result = get_n_dwt_output(0, 9);
    REQUIRE(result == 0);
}

TEST_CASE("get_n_dwt_output(): non-zero input, zero filter", "[dwt][utils]") {
    auto result = get_n_dwt_output(11, 0);
    REQUIRE(result == 0);
}

TEST_CASE("get_n_dwt_output(): one input, one filter", "[dwt][utils]") {
    auto result = get_n_dwt_output(1, 1);
    REQUIRE(result == 0);
}

TEST_CASE("get_n_dwt_output(): even sum of input and filter", "[dwt][utils]") {
    auto result = get_n_dwt_output(5, 5);
    REQUIRE(result == 4);
}

TEST_CASE("get_n_dwt_output(): odd sum of input and filter", "[dwt][utils]") {
    auto result = get_n_dwt_output(12, 7);
    REQUIRE(result == 9);
}

TEST_CASE("get_out_rows_cols()", "[dwt][utils]") {
    struct {
        std::size_t rows{21};
        std::size_t cols{3};
        std::size_t filter_size{7};
    } input;
    auto expected_rows = get_n_dwt_output(input.rows, input.filter_size);
    auto expected_cols = get_n_dwt_output(input.cols, input.filter_size);
    auto get_out_rows_cols_ = [=](auto dwt) {
        return get_out_rows_cols(input.rows,
                                 input.cols,
                                 input.filter_size,
                                 dwt);
    };

    SECTION("non-existing enum value") {
        auto [rows, cols] = get_out_rows_cols_(static_cast<dwt_mode>(-1));
        REQUIRE((rows == 0 && cols == 0));
    }
    SECTION("dwt mode 2d") {
        auto [rows, cols] = get_out_rows_cols_(dwt_mode::_2d);
        REQUIRE((rows == expected_rows && cols == expected_cols));
    }
    SECTION("dwt mode rows") {
        auto [rows, cols] = get_out_rows_cols_(dwt_mode::_rows);
        REQUIRE((rows == input.rows && cols == expected_cols));
    }
    SECTION("dwt mode cols") {
        auto [rows, cols] = get_out_rows_cols_(dwt_mode::_cols);
        REQUIRE((rows == expected_rows && cols == input.cols));
    }
    SECTION("no dwt selected") {
        auto [rows, cols] = get_out_rows_cols_(dwt_mode::_no);
        REQUIRE((rows == input.rows && cols == input.cols));
    }

    SECTION("dwt mode 2d, callback") {
        auto [rows, cols] = get_out_rows_cols_(dwt_2d<float, float>);
        REQUIRE((rows == expected_rows && cols == expected_cols));
    }
    SECTION("dwt mode rows, callback") {
        auto [rows, cols] = get_out_rows_cols_(dwt_2d_rows<float, float>);
        REQUIRE((rows == input.rows && cols == expected_cols));
    }
    SECTION("dwt mode cols, callback") {
        auto [rows, cols] = get_out_rows_cols_(dwt_2d_cols<float, float>);
        REQUIRE((rows == expected_rows && cols == input.cols));
    }
    SECTION("no dwt selected, callback") {
        auto [rows, cols] = get_out_rows_cols_(no_dwt_2d<float, float>);
        REQUIRE((rows == input.rows && cols == input.cols));
    }
}

} // namespace
} // namespace mgr
