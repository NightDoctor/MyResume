#include "common.h"

namespace common
{
long get_current_msec()
{
    auto currentTime = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch()).count();
}
}  // namespace common