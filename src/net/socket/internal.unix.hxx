namespace fscompsoc::net {
  class tcp_socket::__internal_data {
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

  public:
    ~unix_socket() {
      ::close(fd);
      poller.join();
    }
  };
  class tcp_server::__internal_data : public unix_socket {};
  class udp_socket::__internal_data : public unix_socket {};
  class udp_server::__internal_data : public unix_socket {};
}
