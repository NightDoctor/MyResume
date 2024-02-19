#include "worker.h"

int main()
{
    auto server = std::make_shared<udp_server>();

    worker device_worker(server);
    device_worker.init_device();

    server->start();
    return 0;
}
