#include "internal.unix.hxx"

#include "fscompsoc/bits/endian.hpp"
#include "fscompsoc/exceptions.hpp"
#include "fscompsoc/net/socket.hpp"

#include <netinet/in.h>

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

using namespace std;
using namespace fscompsoc::exceptions;
using namespace fscompsoc::bits;
using namespace fscompsoc::async;

namespace fscompsoc::net
{
  tcp_socket::tcp_socket(ip_endpoint server) : __internal(new __internal_data())
  {
    __internal->fd = ::socket(AF_INET, SOCK_STREAM, 0);

    switch (server.addr.version) {
    case 4: {
      sockaddr_in addr;
      addr.sin_family = AF_INET;
      memset(addr.sin_zero, 0, sizeof(addr.sin_zero));
      memcpy(&addr.sin_addr.s_addr, server.addr.bytes.data(), 4);
      addr.sin_port = endian<NetworkEndian, uint16_t>(server.port);

      if (::connect(__internal->fd, reinterpret_cast<sockaddr*>(&addr),
                    sizeof(addr)) == -1)
        throw socket::ConnectionFailed();
    } break;

    case 6: {
      sockaddr_in6 addr;
      addr.sin6_family = AF_INET6;
      memcpy(&addr.sin6_addr.s6_addr, server.addr.bytes.data(), 16);
      addr.sin6_port = endian<NetworkEndian, uint16_t>(server.port);

      if (::connect(__internal->fd, reinterpret_cast<sockaddr*>(&addr),
                    sizeof(addr)) == -1)
        throw socket::ConnectionFailed();
    } break;

    default:
      throw Unsupported("IP version not supported");
    }

    __internal->poller = __internal->create_thread();
  }

  tcp_socket::~tcp_socket() { delete __internal; }

  action<vector<uint8_t>> tcp_socket::receive()
  {
    shared_ptr<bool> c(new bool);

    return action<vector<uint8_t>>(
        [this, c]() { return __internal->get_buffer(c); },
        [this, c]() { __internal->cancel(c); });
  }

  attempt tcp_socket::send(std::vector<uint8_t> data)
  {
    shared_ptr<bool> c(new bool);

    return attempt(
        [this, data = move(data), c]() { return __internal->send(data, c); },
        [this, c]() { __internal->cancel(c); });
  }

  tcp_server::tcp_server(__internal_data* data) : __internal(data) {}

  tcp_server::tcp_server() : tcp_server(new __internal_data()) {}
} // namespace fscompsoc::net
