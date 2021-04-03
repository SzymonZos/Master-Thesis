#include <algorithm>
#include <array>
#include <iostream>
#include <thread>
#include <vector>

#include "dwt.hpp"
#include "luts.hpp"

namespace mgr {

alignas(64) std::atomic<std::size_t> index;
constexpr std::size_t queueSize = 100;

struct atrocity {
    // data
};

constexpr void fillAtrocities() {
    // generate queue array by filling members with right values
}

std::array<std::thread::id, queueSize> queue{};

void threadTesting() {
    //    std::iota(queue.begin(), queue.end(), -100);
    std::vector<std::thread> threads;
    const auto noThreads = std::thread::hardware_concurrency();
    threads.reserve(noThreads);

    for (std::size_t i{}; i < noThreads; i++) {
        threads.emplace_back([]() {
            while (index < queueSize) {
                using namespace std::chrono_literals;
                queue[++index] = std::this_thread::get_id();
                std::this_thread::sleep_for(1ms);
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    std::unordered_map<std::thread::id, std::size_t> counter;
    for (std::size_t i{}; i < queue.size(); i++) {
        std::cout << "[" << i << "]: " << queue[i] << "\n";
        counter[queue[i]]++;
    }
    for (const auto& [id, count] : counter) {
        std::cout << id << ": " << count << "\n";
    }
}

} // namespace mgr

int main() {
    //    mgr::threadTesting();
    std::cout << "\n";
    for (auto x : mgr::detail::lut_db2<float>) {
        std::cout << x << " ";
    }
    std::cout << "\n";
    for (auto x : mgr::lut_db2_f) {
        std::cout << x << " ";
    }
    std::array<float, 4> in{1, 2, 3, 4};
    std::vector<float> out;
    out.reserve(128);
    mgr::downsampling_convolution(in.data(),
                                  in.size(),
                                  mgr::lut_db2_f.data(),
                                  mgr::lut_db2_f.size(),
                                  out.data(),
                                  2,
                                  mgr::padding_mode::symmetric);
    std::cout << "\n";
    for (std::size_t i{}; i < 3; i++) {
        std::cout << out[i] << " ";
    }
    return 0;
}
