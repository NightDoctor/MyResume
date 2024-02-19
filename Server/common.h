#ifndef COMMON_H
#define COMMON_H

#include <chrono>
#include <iostream>

#if __cplusplus >= 201703L
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif

#define SLEEP_MSEC(delay) std::this_thread::sleep_for(std::chrono::milliseconds(delay));

namespace common
{
long get_current_msec();
};  // namespace common

#endif  // COMMON_H
