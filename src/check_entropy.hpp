#ifndef JPEG2000_CHECK_ENTROPY_HPP
#define JPEG2000_CHECK_ENTROPY_HPP

#include <string>

namespace mgr {
void check_entropy_threads(const std::string& path, unsigned int n_threads);
void check_entropy_exec_policies(const std::string& path,
                                 unsigned int n_threads);
} // namespace mgr

#endif // JPEG2000_CHECK_ENTROPY_HPP
