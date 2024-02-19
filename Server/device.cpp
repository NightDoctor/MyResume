#include "device.h"
#include <iostream>

channel::channel(std::string _name) : name(_name), is_thread_running(false), current_range(range_0)
{
    current_state = state::idle_state;
}

void channel::start()
{
    current_state = state::measure_state;
    is_thread_running = true;
    thread_work       = std::thread([this]() {
        std::cout << "thread starting, channel = " << name << " range: " << current_range.min << "--"
                  << current_range.max << std::endl;
        while (is_thread_running)
        {
            std::cout << "thread tick, channel = " << name << std::endl;
            SLEEP_MSEC(1000)
        }
        std::cout << "thread stopping, channel = " << name << std::endl;
    });
    thread_work.detach();
}

void channel::stop()
{
    current_state = state::idle_state;
    is_thread_running = false;
}

void channel::set_range(const std::string& new_range)
{
    if (new_range == "range_0")
        current_range = range_0;
    if (new_range == "range_1")
        current_range = range_1;
    if (new_range == "range_2")
        current_range = range_2;
    if (new_range == "range_3")
        current_range = range_3;
}

std::string channel::get_state_string()
{
    if (current_state == state::idle_state)
        return "idle_state";
    if (current_state == state::measure_state)
        return "measure_state";
    if (current_state == state::busy_state)
        return "busy_state";
    if (current_state == state::error_state)
        return "error_state";

    return "unknown";
}

device::device()
{
}

channel::shared device::get_channel_by_name(const std::string& name)
{
    std::cout << "try find ch " << name << std::endl;
    for (const auto& iter : channel_list)
    {
        std::cout << "iter ch " << iter->get_name() << std::endl;
        if (iter->get_name() == name)
            return iter;
    }

    return nullptr;
}
std::vector<std::shared_ptr<channel>>::iterator device::find_channel(channel::shared obj)
{
    return std::find_if(channel_list.begin(), channel_list.end(),
                        [&obj](const channel::shared& ptr) { return ptr->get_name() == obj->get_name(); });
}
