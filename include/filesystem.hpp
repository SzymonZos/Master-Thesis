#ifndef JPEG2000_FILESYSTEM_HPP
#define JPEG2000_FILESYSTEM_HPP

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace mgr {
inline fs::path operator+(const fs::path& lhs, const fs::path& rhs) {
    fs::path result(lhs);
    return result += rhs;
}

inline fs::path get_path_in_same_dir(const fs::path& current,
                                     const fs::path& other) {
    return current.parent_path() / other;
}

inline fs::path append_to_filename(const fs::path& filename,
                                   const fs::path& ext) {
    return filename.stem() + ext + filename.extension();
}

inline std::string append_iter_to_name(const char* name, std::size_t i) {
    return name + std::to_string(i);
}

} // namespace mgr

#endif // JPEG2000_FILESYSTEM_HPP
