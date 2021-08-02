#include "check_entropy.hpp"

#include <iostream>

namespace mgr {
void check_entropy_threads(const std::string& path, unsigned int n_threads) {
    std::cout << "Path: " << path << ", threads: " << n_threads << "\n";
}
void check_entropy_exec_policies(const std::string& path,
                                 unsigned int n_threads) {
    std::cout << "Path: " << path << ", threads: " << n_threads << "\n";
}
} // namespace mgr
