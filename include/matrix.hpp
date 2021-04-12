#ifndef JPEG2000_MATRIX_HPP
#define JPEG2000_MATRIX_HPP

#include "config.hpp"
#include <initializer_list>
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

    auto n_rows() const {
        return rows_;
    }

    auto n_cols() const {
        return cols_;
    }

    auto data() {
        return buf_.data();
    }

    auto get_row(std::size_t row) {
        return buf_.data() + row * cols_;
    }

    auto get_col(std::size_t col) {
        for (std::size_t i{}; i < col_buf_.size(); i++) {
            col_buf_[i] = buf_[col + i * cols_];
        }
        return col_buf_.data();
    }

private:
    std::size_t rows_{};
    std::size_t cols_{};
    std::vector<T> col_buf_;
    std::vector<T> buf_;
};

} // namespace mgr

#endif // JPEG2000_MATRIX_HPP
