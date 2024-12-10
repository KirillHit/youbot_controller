#include "youbot_lidar_nav/tasks/tcp_server_task.hpp"

#include "youbot_lidar_nav/protocol.hpp"
#include "youbot_lidar_nav/tasks/driver_task.hpp"
#include "youbot_lidar_nav/utils/logger.hpp"
#include "youbot_lidar_nav/utils/parameter_server.hpp"

namespace ybotln {

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
    while ((server.socket_bind() < 0) && !stop_flag)
    {
        LOGGER_STREAM(MSG_LVL::ERROR,
                      "Failed to bind socket! Make sure port " << tcp_server_port << " is free.");
        try_process_commands_for(std::chrono::milliseconds(tcp_server_timeout));
    }
    LOGGER_STREAM(MSG_LVL::INFO, "Tsp server running on port " << tcp_server_port);
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
    if (server.receive(reinterpret_cast<char*>(rx_buffer.data()), RX_MSG_SIZE) <= 0)
    {
        if (con_alive)
        {
            LOGGER_STREAM(MSG_LVL::WARN, "Failed to receive message. Emergency stop");
            send_stop();
            con_alive = false;
        }
        return;
    }
    con_alive = true;

    switch (rx_buffer[0])
    {
        case static_cast<uint8_t>(DataId::GO_ROUTE):
        {
            process_route();
            break;
        }
        default: LOGGER_STREAM(MSG_LVL::WARN, "Wrong data id"); break;
    }
}

void TcpServerTask::send_stop()
{
    auto cmd = std::make_shared<RouteCommand>();
    cmd->set_reset(true);
    RouteStep step {.longitudinal_vel = 0, .transversal_vel = 0, .angular_vel = 0, .duration = 0};
    cmd->add_step(std::move(step));
    emit_command("driver", cmd);
}

void TcpServerTask::process_route()
{
    RouteMsgHeader* route_header = reinterpret_cast<RouteMsgHeader*>(&rx_buffer[1]);
    RouteStepMsg* step_msg = reinterpret_cast<RouteStepMsg*>(&rx_buffer[3]);
    auto cmd = std::make_shared<RouteCommand>();
    cmd->set_reset(route_header->reset_route);
    for (size_t idx = route_header->step_count; idx; --idx)
    {
        RouteStep step {.longitudinal_vel = static_cast<double>(step_msg->longitudinal_vel) / 1000,
                        .transversal_vel = static_cast<double>(step_msg->transversal_vel) / 1000,
                        .angular_vel = static_cast<double>(step_msg->angular_vel) / 1000,
                        .duration = static_cast<int>(step_msg->duration)};
        ++step_msg;
        cmd->add_step(std::move(step));
    }
    emit_command("driver", cmd);
}

}  // namespace ybotln
