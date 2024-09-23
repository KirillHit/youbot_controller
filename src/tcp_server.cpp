#include "youbot_lidar_nav/tcp_server.hpp"
#include "youbot_lidar_nav/utils/logger.hpp"
#include "youbot_lidar_nav/driver.hpp"

namespace ybotln
{

TcpServerTask::TcpServerTask(std::string name) : Task(name)
{
    update_parameters();
    tx_buffer.resize(TX_MSG_SIZE, 0);
    rx_buffer.resize(RX_MSG_SIZE);
}

void TcpServerTask::update_parameters()
{
    tcp_server_port = PARAMETERS.get<int>("tcp_server/port");
    tcp_server_timeout = PARAMETERS.get<int>("tcp_server/timeout");
}

void TcpServerTask::start_server()
{
    LOGGER_STREAM(MSG_LVL::INFO, "Launching the TSP server...");
    server.set_socket("", tcp_server_port);
    server.set_keepalive(1, 1, 1);
    server.set_timeout(tcp_server_timeout);
    server.socket_bind();
    LOGGER_STREAM(MSG_LVL::INFO, "Tsp server running on port " << tcp_server_timeout);
}

void TcpServerTask::task()
{
    start_server();

    while (!stop_flag)
    {
        receive();

        //process_commands();
    }
}

void TcpServerTask::receive()
{
    if (server.receive(reinterpret_cast<char *>(rx_buffer.data()), RX_MSG_SIZE) >= 0)
    {
        LOGGER_STREAM(MSG_LVL::WARN, "Failed to receive message");
        return;
    }

    switch (rx_buffer[0])
    {
    case static_cast<uint8_t>(DataId::GO_ROUTE): {
        RouteStepMsg *step = reinterpret_cast<RouteStepMsg *>(&rx_buffer[2]);
        
        break;
    }
    default:
        LOGGER_STREAM(MSG_LVL::WARN, "Wrong data id");
        break;
    }
}

} // namespace ybotln
