#ifndef JPEG2000_MATRIX_HPP
#define JPEG2000_MATRIX_HPP

#include "config.hpp"
#include "dwt_helpers.hpp"

#include <array>
#include <compare>
#include <initializer_list>
#include <ostream>
#include <vector>

namespace mgr {

template<typename T>
class matrix final {
public:
    matrix() = default;
    ~matrix() = default;
    matrix(const matrix&) = default;
    matrix& operator=(const matrix&) = default;
    matrix(matrix&&) noexcept = default;
    matrix& operator=(matrix&&) noexcept = default;

    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

    matrix(std::size_t rows, std::size_t cols) :
        rows_{rows},
        cols_{cols},
        buf_(rows_ * cols_) {}

    matrix(std::initializer_list<T> list, std::size_t rows, std::size_t cols) :
        rows_{rows},
        cols_{cols},
        buf_(list) {}

    [[nodiscard]] std::size_t rows() const noexcept {
        return rows_;
    }

    [[nodiscard]] std::size_t cols() const noexcept {
        return cols_;
    }

    const_pointer data() const noexcept {
        return buf_.data();
    }

    pointer data() noexcept {
        return buf_.data();
    }

    const_pointer get_row(std::size_t row) const noexcept {
        return buf_.data() + row * cols_;
    }

    reference operator[](std::size_t index) noexcept {
        return buf_[index];
    }

    const_reference operator[](std::size_t index) const noexcept {
        return buf_[index];
    }

    friend std::ostream& operator<<(std::ostream& os, const matrix<T>& mat) {
        os << "[ ";
        for (std::size_t i{}; i < mat.rows_ * mat.cols_; i++) {
            if (i && i % mat.cols_ == 0) {
                os << "\n";
            }
            os << mat[i] << " ";
        }
        os << "\b]";
        return os;
    }

    auto operator<=>(const matrix&) const = default;

private:
    std::size_t rows_{};
    std::size_t cols_{};
    std::vector<T> buf_;
};

template<typename T, std::size_t N>
matrix<T> dwt_2d_wrapper(const matrix<T>& input,
                         const std::array<T, N>& filter,
                         dwt_2d_cb<T, T> dwt_cb,
                         padding_mode mode) {
    auto [rows_out, cols_out] = get_out_rows_cols(input.rows(),
                                                  input.cols(),
                                                  filter.size(),
                                                  dwt_cb);
    matrix<T> output(rows_out, cols_out);
    dwt_cb(input.data(),
           input.rows(),
           input.cols(),
           filter.data(),
           filter.size(),
           output.data(),
           mode);
    return output;
}

} // namespace mgr

#endif // JPEG2000_MATRIX_HPP
