#include "fscompsoc/net/socket.hpp"
#include "fscompsoc/exceptions.hpp"
#include "fscompsoc/bits/endian.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>

using namespace std;
using namespace fscompsoc::exceptions;
using namespace fscompsoc::bits;

namespace fscompsoc::net {
  class unix_socket {
  public:
    int fd;
  };

  class tcp_socket::__internal_data : public unix_socket {};
  class tcp_server::__internal_data : public unix_socket {};
  class udp_socket::__internal_data : public unix_socket {};
  class udp_server::__internal_data : public unix_socket {};

  tcp_socket::tcp_socket(ip_endpoint server) : __internal(new __internal_data()) {
    __internal->fd = ::socket(AF_INET, SOCK_STREAM, 0);

    switch (server.addr.version) {
      case 4:
        {
          sockaddr_in addr;
          addr.sin_family = AF_INET;
          memcpy(&addr.sin_addr.s_addr, server.addr.bytes.data(), 4);
          addr.sin_port = endian<NetworkEndian, uint16_t>(server.port);

          ::connect(
            __internal->fd,
            reinterpret_cast<sockaddr*>(&addr),
            sizeof(addr)
          );
        }
        break;

      case 6:
        {
          sockaddr_in6 addr;
          addr.sin6_family = AF_INET6;
          memcpy(&addr.sin6_addr.s6_addr, server.addr.bytes.data(), 16);
          addr.sin6_port = endian<NetworkEndian, uint16_t>(server.port);

          ::connect(
            __internal->fd,
            reinterpret_cast<sockaddr*>(&addr),
            sizeof(addr)
          );
        }
      break;

    default:
      throw Unsupported("IP version not supported");
    }
  }

  tcp_socket::~tcp_socket() {
      ::close(__internal->fd);
      delete __internal;
  }

  async::maybe<std::vector<uint8_t>> receive() {
    
  }
}
