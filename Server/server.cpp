#include "server.h"

#include <cstring>
#include <sstream>

udp_server::udp_server() : is_running(false)
{
    input_data = new uint8_t[BUFFER_SIZE];
}

udp_server::~udp_server()
{
    stop();
    delete input_data;
}

void udp_server::start()
{
    is_running             = true;
    server_thread          = std::thread([this]() {
        std::cout << "UDP Server started..." << std::endl;
        open_server();
    });
    message_handler_thread = std::thread([this]() { handle_messages(); });
    message_sender_thread  = std::thread([this]() { send_messages(); });
    message_handler_thread.detach();
    message_sender_thread.detach();
    server_thread.join();
}

void udp_server::stop()
{
    if (is_running)
        is_running = false;

    close(PORT);
}

uint32_t udp_server::open_server()
{
    uint32_t server_fd;
    struct sockaddr_in address
    {
    };
    int addrlen = sizeof(address);

    close(PORT);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }

    int flags = fcntl(server_fd, F_GETFL, 0);
    if (flags == -1)
    {
        std::cerr << "Error getting socket flags" << std::endl;
        return 1;
    }
    if (fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        std::cerr << "Error setting socket to non-blocking mode" << std::endl;
        return 1;
    }

    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port        = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        std::cerr << "Bind failed: " << std::strerror(errno) << std::endl;
        return 1;
    }

    if (listen(server_fd, 3) < 0)
    {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }

    timeval timeout;
    timeout.tv_sec  = 0;
    timeout.tv_usec = 100'000;
    if (setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0)
    {
        std::cerr << "Error setting socket receive timeout" << std::endl;
        return 1;
    }

    while (is_running)
    {
        auto new_socket_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (new_socket_fd == 0xffffffff)
            continue;

        std::cout << "New connection accepted " << new_socket_fd << std::endl;

        mutex_clients.lock();
        clients.emplace_back(new_socket_fd, common::get_current_msec());
        mutex_clients.unlock();

        if (m_client_handler)
            m_client_handler(new_socket_fd);
        SLEEP_MSEC(100)
    }
    close(server_fd);
    close(PORT);
    return -1;
}

void udp_server::handle_messages()
{
    while (is_running)
    {
        auto time_now = common::get_current_msec();
        std::lock_guard<std::mutex> lock(mutex_clients);
        auto client = clients.begin();
        while (client != clients.end())
        {
            auto difference = time_now - client->last_ping;
            if (difference > 5000)
            {
                std::cout << "ping difference=" << difference << " now=" << time_now << " cli=" << client->last_ping
                          << std::endl;
                client_disconnect(client);
                continue;
            }
            int valread = recv(client->socket_id, input_data, BUFFER_SIZE, 0);
            if (valread > 0)
            {
                std::cout << "new message size=" << valread << std::endl;
                auto type = api::handle_api_packet(input_data, valread, message_request_vec);
                if (type == api::type::ping)
                {
                    client->last_ping = common::get_current_msec();
                    std::cout << "ping id=" << client->socket_id << std::endl;
                }
                else if (m_message_handler)
                {
                    if (type != api::type::unknown)
                        m_message_handler(client->socket_id, type, message_request_vec);
                    std::cout << "message handled" << std::endl;
                }
            }
            else if (valread == -1 && errno != EWOULDBLOCK && errno != EAGAIN)
            {
                std::cout << "err handle message, id=" << client->socket_id << std::endl;
            }
            ++client;
            SLEEP_MSEC(1000)
        }
    }
}

void udp_server::set_client_handler(client_handler _client_handler)
{
    m_client_handler = _client_handler;
}

void udp_server::set_message_handler(message_handler _message_handler)
{
    m_message_handler = _message_handler;
}

void udp_server::send_messages()
{
    while (is_running)
    {
        mutex_messages.lock();
        if (!messages.empty())
        {
            auto message = messages.front();
            for (auto& client : clients)
            {
                if (client.socket_id == message.first)
                {
                    int send_len = write(client.socket_id, message.second.c_str(), message.second.length());
                    if (send_len < 0)
                        std::cerr << "Error sending message to client\n";
                    else
                        std::cout << "message sent: " << message.second << " cli:" << client.socket_id << std::endl;
                }
            }
            messages.pop();
        }
        mutex_messages.unlock();
        SLEEP_MSEC(100)
    }
}

void udp_server::add_msg_to_send(uint32_t client_id, std::string message)
{
    messages.push({ client_id, message });
}

void udp_server::client_disconnect(std::vector<connector::client>::iterator& client)
{
    if (client != clients.end())
    {
        close(client->socket_id);
        std::cerr << "client disconnected id = " << client->socket_id << std::endl;
        client = clients.erase(client);
    }
}

api::type api::handle_api_packet(uint8_t* data, size_t size, std::vector<std::string>& message_request_vec)
{
    message_request_vec.clear();
    if (size < 1 || data == nullptr)
    {
        std::cerr << "incorrect message buffer" << std::endl;
        return api::type::unknown;
    }

    auto* buffer = new uint8_t[size];
    std::memcpy(buffer, data, size);

    std::string packet(reinterpret_cast<char*>(buffer), size);
    size_t pos = packet.find('\n');
    if (pos == std::string::npos)
    {
        std::cerr << "error packet" << std::endl;
        return api::type::unknown;
    }
    else
        packet.erase(pos);

    if (packet == "ping")
        return api::type::ping;

    size_t space_pos = packet.find(' ');

    if (space_pos != std::string::npos)
    {
        auto packet_type_str = packet.substr(0, space_pos);
        auto type            = api::get_type_by_name(packet_type_str);
        if (type == type::unknown)
        {
            std::cerr << "unknown input message type " << packet_type_str << std::endl;
            return type;
        }

        auto message_body = packet.substr(space_pos + 1);
        message_body.erase(std::remove(message_body.begin(), message_body.end(), ' '), message_body.end());

        std::istringstream ss(message_body);
        std::string token;
        while (std::getline(ss, token, ','))
            message_request_vec.push_back(token);

        return type;
    }

    delete[] buffer;
    return api::type::unknown;
}