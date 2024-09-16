#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <vector>

#include "simple_socket/simple_socket.hpp"
#include "protocol.hpp"

namespace ybotln
{

class TcpServer
{
  public:
    TcpServer();
    ~TcpServer() = default;

    void start();
    void stop();

  private:

    void receive();
    sockets::TCPServer server;
    std::vector<uint8_t> tx_buffer;
    std::vector<uint8_t> rx_buffer;
};

} // namespace ybotln

#endif
