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
#include <vector>

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
    mutex socket_mutex;

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
          socket_mutex.lock();
          switch (result) {
            case 1: {
              // Success, read data
              int length = ioctl(fd, FIONREAD, &length);

              buffers_mutex.lock();

              vector<uint8_t> buffer(length);

              length = ::read(fd, buffer.data(), length);

              if(length < 0)
                throw InternalException(Insane("recv < 0 for a checked read"));

              buffer.resize(length);

              buffers_mutex.unlock();

              condvar.notify_one();
            }
            break;

            default: {
              // Failure, stop looping
              loop = false;
            }
            break;
          }
          socket_mutex.unlock();
        }
      });
    }

    optional<vector<uint8_t>> get_buffer(shared_ptr<bool> cancel) {
      unique_lock<mutex> lock(condvar_mutex);
      optional<vector<uint8_t>> ret;
      while(condvar.wait_for(lock, 10ms) == cv_status::timeout) {
        if(*cancel)
          return nullopt;
      }

      buffers_mutex.lock();

      // Check for spurious wakeup and final cancel check
      if(buffers.size() > 0 && !*cancel) {
        ret = buffers.front();
        buffers.pop();
      }

      buffers_mutex.unlock();

      return ret;
    }

    void cancel(shared_ptr<bool> cancel) {
      *cancel = true;
    }

    bool send(std::vector<uint8_t> data, shared_ptr<bool> cancel) {
      socket_mutex.lock();
      bool success = false;
      if(!*cancel)
        success = (::write(fd, data.data(), data.size()) != -1);
      socket_mutex.unlock();

      return success;
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
    shared_ptr<bool> c(new bool);

    return action<vector<uint8_t>>(
      [this, c]() { return __internal->get_buffer(c); },
      [this, c]() { __internal->cancel(c); }
    );
  }

  attempt tcp_socket::send(std::vector<uint8_t> data) {
    shared_ptr<bool> c(new bool);

    return attempt(
      [this, data = move(data), c]() { return __internal->send(data, c); },
      [this, c]() { __internal->cancel(c); }
    );
  }
}
