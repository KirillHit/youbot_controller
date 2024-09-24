#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <vector>

#include "protocol.hpp"
#include "simple_socket/simple_socket.hpp"
#include "utils/task_pool.hpp"

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

    sockets::TCPServer server;
    std::vector<uint8_t> tx_buffer;
    std::vector<uint8_t> rx_buffer;
    int tcp_server_port;
    int tcp_server_timeout;

};

} // namespace ybotln

#endif
