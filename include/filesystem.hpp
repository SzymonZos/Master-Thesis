#ifndef JPEG2000_FILESYSTEM_HPP
#define JPEG2000_FILESYSTEM_HPP

#include <filesystem>

namespace fs = std::filesystem;

namespace mgr {
fs::path operator+(const fs::path& lhs, const fs::path& rhs) {
    fs::path result(lhs);
    return result += rhs;
}
} // namespace mgr

#endif // JPEG2000_FILESYSTEM_HPP
