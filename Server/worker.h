#ifndef WORKER_H
#define WORKER_H

#include "device.h"
#include "server.h"

class worker
{
public:
    worker(std::shared_ptr<udp_server>);

    void init_device();

    void add_new_client(uint32_t);
    bool try_start(channel::shared);
    bool try_stop(channel::shared);

private:
    std::shared_ptr<udp_server> server_lnk;

    std::vector<uint32_t> clients;
    device system_device;
};

#endif  // WORKER_H
