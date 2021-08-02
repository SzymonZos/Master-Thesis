#include "arg_parser.hpp"

#include "check_entropy.hpp"
#include "demo_dwt.hpp"
#include "demo_opencv.hpp"
#include "demo_queue.hpp"

#include <thread>

#include <cxxopts.hpp>

namespace mgr {
void parse_args(int argc, char** argv) {
    try {
        cxxopts::Options options(argv[0], "Demo of jpeg 2000 project");
        options.positional_help("[optional args]").show_positional_help();

        // clang-format off
        options
            .allow_unrecognised_options()
            .add_options()
                ("d,dwt", "Demo of basic dwt usages")
                ("q,queue", "Demo of dwt processing queue usages")
                ("filters", "Demo of filters used")
                ("dwt-1d", "Demo of 1d dwt usage")
                ("dwt-2d", "Demo of 2d dwt usage")
                ("sequential", "Demo of sequential queue")
                ("parallel", "Demo of parallel queue")
                ("my-parallel", "Demo of my parallel queue")
                ("opencv", "Demo of using opencv loading image", cxxopts::value<std::string>())
                ("opencv-queue", "Demo of dwt queue with opencv loading image", cxxopts::value<std::string>())
                ("opencv-queue-parallel", "Demo of parallel dwt queue with opencv loading image",
                    cxxopts::value<std::string>())
                ("opencv-queue-entropy", "Demo of dwt queue with opencv and entropy", cxxopts::value<std::string>())
                ("opencv-queue-parallel-rgb", "Demo of parallel dwt queue with opencv for rgb images",
                    cxxopts::value<std::string>())
                ("threads", "Number of used threads", cxxopts::value<unsigned int>())
                ("check-entropy-threads", "Check which dwt decomposition has lowest entropy with threads",
                    cxxopts::value<std::string>())
                ("check-entropy-exec-policies", "Check which dwt decomposition has lowest entropy with exec policies",
                 cxxopts::value<std::string>())
                ("h,help", "Print help");

        // clang-format on

        auto result = options.parse(argc, argv);
        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            exit(0);
        }
        if (result.count("d")) {
            demo_filter_luts();
            demo_dwt_1d();
            demo_dwt_2d();
        }
        if (result.count("q")) {
            demo_sequential();
            demo_parallel();
            demo_my_parallel();
        }
        if (result.count("filters")) {
            demo_filter_luts();
        }
        if (result.count("dwt-1d")) {
            demo_dwt_1d();
        }
        if (result.count("dwt-2d")) {
            demo_dwt_2d();
        }
        if (result.count("sequential")) {
            demo_sequential();
        }
        if (result.count("parallel")) {
            demo_parallel();
        }
        if (result.count("my-parallel")) {
            demo_my_parallel();
        }
        if (result.count("opencv")) {
            demo_opencv(result["opencv"].as<std::string>());
        }
        if (result.count("opencv-queue")) {
            demo_opencv_queue(result["opencv-queue"].as<std::string>());
        }
        if (result.count("opencv-queue-parallel")) {
            demo_opencv_parallel_queue(
                result["opencv-queue-parallel"].as<std::string>());
        }
        if (result.count("opencv-queue-entropy")) {
            demo_opencv_entropy(
                result["opencv-queue-entropy"].as<std::string>());
        }
        if (result.count("opencv-queue-parallel-rgb")) {
            demo_opencv_parallel_queue_rgb(
                result["opencv-queue-parallel-rgb"].as<std::string>());
        }
        unsigned int n_threads = std::thread::hardware_concurrency();
        if (result.count("threads")) {
            n_threads = result["threads"].as<unsigned int>();
        }
        if (result.count("check-entropy-threads")) {
            auto path = result["check-entropy-threads"].as<std::string>();
            check_entropy_threads(path, n_threads);
        }
        if (result.count("check-entropy-exec-policies")) {
            auto p = result["check-entropy-exec-policies"].as<std::string>();
            check_entropy_exec_policies(p, n_threads);
        }

    } catch (const cxxopts::OptionException& e) {
        std::cout << "error parsing options: " << e.what() << std::endl;
        exit(1);
    }
}
} // namespace mgr
