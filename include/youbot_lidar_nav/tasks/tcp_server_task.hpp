#ifndef TCP_SERVER_TASK_HPP
#define TCP_SERVER_TASK_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "simple_socket/simple_socket.hpp"
#include "youbot_lidar_nav/utils/task_pool.hpp"

namespace ybotln
{

class TcpServerTask : public Task
{
  public:
    TcpServerTask(std::string name);
    ~TcpServerTask() = default;
    void update_parameters();

  private:
    void task() override;
    void start_server();
    void receive();
    void process_route();
    void send_stop();

    sockets::TCPServer server;
    std::vector<uint8_t> tx_buffer;
    std::vector<uint8_t> rx_buffer;
    int tcp_server_port;
    int tcp_server_timeout;
    bool con_alive = false;
};

} // namespace ybotln

#endif
