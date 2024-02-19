#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include <fcntl.h>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>
#include "client.h"

constexpr int BUFFER_SIZE = 1024;
constexpr int PORT        = 5678;

namespace api
{
enum class type
{
    start_measure = 0x00,
    set_range     = 0x01,
    stop_measure  = 0x02,
    get_status    = 0x03,
    get_result    = 0x04,
    ping          = 0x04,  // need message rx
    unknown       = 0xff
};

inline std::string get_type_str(type _type)
{
    switch (_type)
    {
        case type::start_measure:
            return "start_measure";
        case type::set_range:
            return "set_range";
        case type::stop_measure:
            return "stop_measure";
        case type::get_status:
            return "get_status";
        case type::get_result:
            return "get_result";
        default:
            break;
    }
    return "unknown";
}

inline type get_type_by_name(const std::string& name)
{
    if (name == "start_measure")
        return type::start_measure;
    if (name == "set_range")
        return type::set_range;
    if (name == "stop_measure")
        return type::stop_measure;
    if (name == "get_status")
        return type::get_status;
    if (name == "get_result")
        return type::get_result;

    return type::unknown;
}

type handle_api_packet(uint8_t*, size_t, std::vector<std::string>&);
}  // namespace api

class udp_server
{
public:
    using message_handler = std::function<void(uint32_t, api::type, std::vector<std::string>)>;
    using client_handler  = std::function<void(uint32_t)>;

public:
    udp_server();
    ~udp_server();

    void start();
    void stop();
    uint32_t open_server();
    void add_msg_to_send(uint32_t, std::string);

    void send_messages();
    void handle_messages();

    void set_message_handler(message_handler);
    void set_client_handler(client_handler);
    void client_disconnect(std::vector<connector::client>::iterator&);

private:
    std::atomic<bool> is_running;
    std::thread server_thread;
    std::vector<connector::client> clients;

    std::thread message_handler_thread;
    std::thread message_sender_thread;

    std::mutex mutex_clients;

    std::mutex mutex_messages;
    std::queue<std::pair<uint32_t, std::string>> messages;

    message_handler m_message_handler;
    client_handler m_client_handler;

    uint8_t* input_data;
    std::vector<std::string> message_request_vec;
};

#endif  // SERVER_H
