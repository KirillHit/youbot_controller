#include "youbot_lidar_nav/tcp_server.hpp"
#include "youbot_lidar_nav/driver.hpp"
#include "youbot_lidar_nav/utils/logger.hpp"

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

        try_process_commands();
    }
}

void TcpServerTask::receive()
{
    if (server.receive(reinterpret_cast<char *>(rx_buffer.data()), RX_MSG_SIZE) < 0)
    {
        LOGGER_STREAM(MSG_LVL::DEBUG, "Failed to receive message");
        return;
    }

    switch (rx_buffer[0])
    {
    case static_cast<uint8_t>(DataId::GO_ROUTE): {
        RouteStepMsg *step_msg = reinterpret_cast<RouteStepMsg *>(&rx_buffer[2]);
        auto cmd = std::make_shared<RouteCommand>();
        for (size_t idx = rx_buffer[2]; idx; --idx)
        {
            RouteStep step{.longitudinal_vel =
                               static_cast<double>(step_msg->longitudinal_vel) / 1000,
                           .transversal_vel = static_cast<double>(step_msg->transversal_vel) / 1000,
                           .angular_vel = static_cast<double>(step_msg->angular_vel) / 1000,
                           .duration = static_cast<int>(step_msg->duration)};
            cmd->add_step(std::move(step));
            ++step_msg;
        }
        emit_command("driver", cmd);
        break;
    }
    default:
        LOGGER_STREAM(MSG_LVL::WARN, "Wrong data id");
        break;
    }
}

} // namespace ybotln
