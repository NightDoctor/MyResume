#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>

namespace settings
{
constexpr uint8_t devices_count = 2;
static_assert(devices_count >= 2, "Devices_count should be >= 2");
};  // namespace settings

#endif  // CONFIG_H
