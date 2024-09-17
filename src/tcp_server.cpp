#include "youbot_lidar_nav/tcp_server.hpp"
#include "youbot_lidar_nav/utils/logger.hpp"

namespace ybotln
{

TcpServer::TcpServer()
{
    LOGGER_STREAM(MSG_LVL::INFO, "Launching the TSP server...");
    server.set_socket("", PARAMETERS.get<int>("tcp_server_port"));
    server.set_keepalive(1, 1, 1);
    server.set_timeout(PARAMETERS.get<int>("tcp_server_timeout"));
    server.socket_bind();
    LOGGER_STREAM(MSG_LVL::INFO, "Tsp server running on port "
                                     << PARAMETERS.get<int>("tcp_server_port"));

    tx_buffer.resize(TX_MSG_SIZE, 0);
    rx_buffer.resize(RX_MSG_SIZE);
}

void TcpServer::start()
{
    
}

void TcpServer::stop()
{

}

void TcpServer::receive()
{
    if (server.receive(reinterpret_cast<char *>(rx_buffer.data()),
                       RX_MSG_SIZE) > 0)
    {
        LOGGER_STREAM(MSG_LVL::WARN, "Failed to receive message");
        return;
    }

    switch (rx_buffer[0])
    {
    case static_cast<uint8_t>(DataId::GO_ROUTE):
        // TODO
        break;

    default:
        LOGGER_STREAM(MSG_LVL::WARN, "Wrong data id");
        break;
    }
}

} // namespace ybotln
