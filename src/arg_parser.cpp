#include "arg_parser.hpp"

#include "demo_dwt.hpp"
#include "demo_opencv.hpp"
#include "demo_queue.hpp"

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

    } catch (const cxxopts::OptionException& e) {
        std::cout << "error parsing options: " << e.what() << std::endl;
        exit(1);
    }
}
} // namespace mgr
