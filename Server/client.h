#ifndef CLIENT_H
#define CLIENT_H

#include <cstdint>

#include "common.h"

namespace connector
{
struct client
{
    uint32_t socket_id;
    long last_ping;
    client(uint32_t socket_id_, long last_ping_) : socket_id(socket_id_), last_ping(last_ping_)
    {
    }
};
}  // namespace connector

#endif  // CLIENT_H
