#ifndef DEVICE_H
#define DEVICE_H

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "common.h"

enum class state : uint8_t
{
    idle_state    = 0x00,
    measure_state = 0x01,
    busy_state    = 0x02,
    error_state   = 0xff
};

enum class channel_measure_event : uint8_t
{
    start   = 0x00,
    started = 0x01,
    stop    = 0x02,
    stopped = 0x03
};

class channel
{
public:
    channel(std::string _name);

private:
    struct voltage_range
    {
        double min;
        double max;
    };

public:
    NODISCARD inline const std::string get_name()
    {
        return name;
    }

    void start();
    void stop();

    void set_range(const std::string&);
    std::string get_state_string();

private:
#define RANGE_0 voltage_range range_0{ 0.0000001, 0.001 };
#define RANGE_1 voltage_range range_1{ 0.001, 1 };
#define RANGE_2 voltage_range range_2{ 1, 1000 };
#define RANGE_3 voltage_range range_3{ 1000, 1000000 };

    RANGE_0
    RANGE_1
    RANGE_2
    RANGE_3

    std::string name;
    state current_state;
    float current_voltage;

    voltage_range current_range;

public:
    bool is_thread_running;
    std::thread thread_work;

    channel_measure_event current_measure_event;
    typedef std::shared_ptr<channel> shared;
};

struct device
{
    std::vector<channel::shared> channel_list;

    device();
    ~device() = default;

    channel::shared get_channel_by_name(const std::string&);
    std::vector<std::shared_ptr<channel>>::iterator find_channel(channel::shared);
};

#endif  // DEVICE_H
