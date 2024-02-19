#include <iostream>
#include <utility>

#include "config.h"
#include "worker.h"

worker::worker(std::shared_ptr<udp_server> _server)
{
    server_lnk = std::move(_server);
    server_lnk->set_message_handler([&](uint32_t socket_id, api::type _type, std::vector<std::string> request_str) {
        if (_type == api::type::get_status)
        {
            if (request_str.empty())
            {
                server_lnk->add_msg_to_send(socket_id, "fail, err mess\n");
                return;
            }
            else
                std::cout << "request_str not empty" << std::endl;

            auto channel_name = request_str.at(0);
            std::cout << "channel_name " << channel_name << std::endl;
            auto ret_channel = system_device.get_channel_by_name(channel_name);
            std::cout << "ret_channel " << ret_channel << std::endl;
            if (!ret_channel)
            {
                std::cout << "channel " << channel_name << " not found" << std::endl;
                server_lnk->add_msg_to_send(socket_id, "fail, err mess\n");
                return;
            }

            std::cout << "ret_channel is name " << ret_channel->get_name() << std::endl;
            server_lnk->add_msg_to_send(socket_id, "ok, " + ret_channel->get_state_string() + '\n');
            return;
        }
        else if (_type == api::type::set_range)
        {
            if (request_str.size() < 2)
            {
                server_lnk->add_msg_to_send(socket_id, "fail, err mess\n");
                return;
            }

            auto channel_name = request_str.at(0);
            auto range_str    = request_str.at(1);
            auto ret_channel  = system_device.get_channel_by_name(channel_name);
            if (!ret_channel)
            {
                std::cout << "channel " << channel_name << "not found";
                server_lnk->add_msg_to_send(socket_id, "fail\n");
                return;
            }
            ret_channel->set_range(range_str);
            server_lnk->add_msg_to_send(socket_id, "ok, " + range_str + "\n");
            return;
        }
        else if (_type == api::type::start_measure)
        {
            if (request_str.empty())
            {
                server_lnk->add_msg_to_send(socket_id, "fail, err mess\n");
                return;
            }
            else
                std::cout << "request_str not empty" << std::endl;

            auto channel_name = request_str.at(0);
            std::cout << "channel_name " << channel_name << std::endl;
            auto ret_channel = system_device.get_channel_by_name(channel_name);
            std::cout << "ret_channel " << ret_channel << std::endl;
            if (!ret_channel)
            {
                std::cout << "channel " << channel_name << " not found" << std::endl;
                server_lnk->add_msg_to_send(socket_id, "fail, err mess\n");
                return;
            }

            if (try_start(ret_channel))
                server_lnk->add_msg_to_send(socket_id, "ok\n");
            else
                server_lnk->add_msg_to_send(socket_id, "fail\n");
            return;
        }
        else if (_type == api::type::stop_measure)
        {
            if (request_str.empty())
            {
                server_lnk->add_msg_to_send(socket_id, "fail");
                return;
            }

            auto channel_name = request_str.at(0);
            auto ret_channel  = system_device.get_channel_by_name(channel_name);
            if (!ret_channel)
            {
                std::cout << "channel " << channel_name << "not found";
                server_lnk->add_msg_to_send(socket_id, "fail");
                return;
            }
            if (try_stop(ret_channel))
                server_lnk->add_msg_to_send(socket_id, "ok\n");
            else
                server_lnk->add_msg_to_send(socket_id, "fail\n");
            return;
        }
        else if (_type == api::type::get_result)
        {
            if (request_str.empty())
            {
                server_lnk->add_msg_to_send(socket_id, "fail, err mess\n");
                return;
            }
            server_lnk->add_msg_to_send(socket_id, "ok, \n");
            return;
        }
    });
    server_lnk->set_client_handler([&](uint32_t socket_id) {
        add_new_client(socket_id);
        std::cout << "Attention, new client id=" << std::to_string(socket_id) << std::endl;
    });
}

void worker::init_device()
{
    for (uint8_t i = 0; i < settings::devices_count; i++)
    {
        std::string name = "channel" + std::to_string(i);
        system_device.channel_list.push_back(std::make_shared<channel>(channel{ name }));
    }
}

void worker::add_new_client(uint32_t new_socket)
{
    //    clients.push_back(new_socket);
    //    for (const auto& iter : system_device.channel_list)
    //    {
    //        int send_len = write(new_socket, iter->name.c_str(), iter->name.length());
    //        if (send_len < 0)
    //        {
    //            std::cerr << "Error sending message to client\n";
    //        }
    //    }
}

bool worker::try_start(channel::shared sharedPtr)
{
    auto ret_val = system_device.find_channel(sharedPtr);
    if (ret_val != system_device.channel_list.end())
    {
        auto current_channel = ret_val->get();
        if (!current_channel->is_thread_running)
        {
            ret_val->get()->start();
            return true;
        }
        else
        {
            std::cout << "the channel is already started" << std::endl;
            return false;
        }
    }
    return false;
}

bool worker::try_stop(channel::shared sharedPtr)
{
    auto ret_val = system_device.find_channel(sharedPtr);
    if (ret_val != system_device.channel_list.end())
    {
        auto current_channel = ret_val->get();
        if (current_channel->is_thread_running)
        {
            ret_val->get()->stop();
            return true;
        }
        else
        {
            std::cout << "the channel is already stopped" << std::endl;
            return false;
        }
    }
    return false;
}
