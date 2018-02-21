#include "net/socket.hpp"
#include "exceptions.hpp"
#include "bits/endian.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
          addr.sin_port = from_host_endian<NetworkEndian, uint16_t>(server.port);

        }
        break;

      case 6:
        {

        }
      break;

    default:
      throw Unsupported("IP version not supported");
    }
  }
}
