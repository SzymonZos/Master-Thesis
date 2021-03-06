#ifndef JPEG2000_DEMO_OPENCV_HPP
#define JPEG2000_DEMO_OPENCV_HPP

#include <string>

namespace mgr {

void demo_opencv(const std::string& path);
void demo_opencv_queue(const std::string& path);
void demo_opencv_parallel_queue(const std::string& path);
void demo_opencv_entropy(const std::string& path);
void demo_opencv_parallel_queue_rgb(const std::string& path);

} // namespace mgr

#endif // JPEG2000_DEMO_OPENCV_HPP
