#include "fscompsoc/net/socket.hpp"
#include "fscompsoc/exceptions.hpp"
#include "fscompsoc/bits/endian.hpp"

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>

using namespace std;
using namespace fscompsoc::exceptions;
using namespace fscompsoc::bits;
using namespace fscompsoc::async;

namespace fscompsoc::net {
  class unix_socket {
  public:
    int fd;
    thread poller;
    queue<vector<uint8_t>> buffers;
    mutex buffers_mutex;
    condition_variable condvar;
    // Damn spurious wakeups...
    mutex condvar_mutex;

  public:
    thread create_thread() {
      return thread([this]() {
        pollfd p;
        p.fd = fd;
        p.events = POLLIN;
        p.revents = 0;

        bool loop = false;

        while(loop) {
          int result = poll(&p, 1, 0);

          switch (result) {
            case 1: {
              // Success, read data
              int length = ioctl(fd, FIONREAD, &length);

              buffers_mutex.lock();

              vector buffer(length);

              length = ::read(fd, buffer.data() + buffer_size, length);

              if(length < 0)
                throw InternalException(Insane("recv < 0 for a checked read"));

              buffer.resize(buffer_size + length);

              buffer_mutex.unlock();

              condvar.notify_one();
            }
            break;

            default: {
              // Failure, stop looping
              loop = false;
            }
            break;
          }
        }
      });
    }
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

    __internal->poller = __internal->create_thread();
  }

  tcp_socket::~tcp_socket() {
    ::close(__internal->fd);
    delete __internal;
  }

  action<vector<uint8_t>> tcp_socket::receive() {
    return function<optional<vector<uint8_t>>()>([this]() {
      unique_lock<mutex> lock(condvar_mutex);
      condvar.wait(lock);

      mutex.lock();

      mutex.unlock();
    });
  }
}
