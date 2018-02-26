#include "internal.unix.hxx"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

using namespace fscompsoc::exceptions;

namespace fscompsoc::net
{
  thread tcp_server::__internal_data::create_thread()
  {
    return thread([this]() {
      pollfd p;
      p.fd      = fd;
      p.events  = POLLIN;
      p.revents = 0;

      bool loop = false;

      while (loop) {
        int result = poll(&p, 1, 0);
        switch (result) {
        case 1: {
          // Success, accept client
          fds_mutex.lock();
          int client = ::accept(fd, nullptr, nullptr);
          if (client != -1)
            fds.push(client);
        } break;

        default: {
          // Failure, stop looping
          loop = false;
        } break;
        }
      }
    });
  }

  std::optional<std::unique_ptr<tcp_socket>>
  tcp_server::__internal_data::accept(std::shared_ptr<bool> cancel)
  {
    unique_lock<mutex> lock(condvar_mutex);
    condvar.wait(lock);

    fds_mutex.lock();
    if (fds.size() && cancel) {
      // return;
    }
    fds_mutex.unlock();
  }

  tcp_server::__internal_data::~__internal_data()
  {
    ::close(fd);
    poller.join();
  }
} // namespace fscompsoc::net
