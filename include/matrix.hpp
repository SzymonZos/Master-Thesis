#ifndef JPEG2000_MATRIX_HPP
#define JPEG2000_MATRIX_HPP

#include "config.hpp"
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
        col_buf_(rows_),
        buf_(rows_ * cols_) {}

    matrix(std::initializer_list<T> list, std::size_t rows, std::size_t cols) :
        rows_{rows},
        cols_{cols},
        col_buf_(rows_),
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

    const_pointer get_col(std::size_t col) noexcept {
        for (std::size_t i{}; i < col_buf_.size(); i++) {
            col_buf_[i] = buf_[col + i * cols_];
        }
        return col_buf_.data();
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

private:
    std::size_t rows_{};
    std::size_t cols_{};
    std::vector<T> col_buf_;
    std::vector<T> buf_;
};

} // namespace mgr

#endif // JPEG2000_MATRIX_HPP
