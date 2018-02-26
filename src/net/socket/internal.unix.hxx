#pragma once

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "fscompsoc/net/socket.hpp"

#include "fscompsoc/async/cancel.hpp"

namespace fscompsoc::net
{
  class unix_socket
  {
  public:
    int fd;
    std::thread poller;
    std::condition_variable condvar;
    // Damn spurious wakeups...
    std::mutex condvar_mutex;

  public:
    virtual std::thread create_thread() = 0;

    virtual void cancel(std::shared_ptr<bool> cancel) = 0;
  };

  class tcp_socket::__internal_data final : public unix_socket
  {
  public:
    std::mutex socket_mutex;
    std::queue<std::vector<uint8_t>> buffers;
    std::mutex buffers_mutex;

  public:
    std::thread create_thread() final;

    std::optional<std::vector<uint8_t>>
    get_buffer(std::shared_ptr<bool> cancel);

    void cancel(std::shared_ptr<bool> cancel) final;

    bool send(std::vector<uint8_t> data, std::shared_ptr<bool> cancel);

  public:
    ~__internal_data();
  };

  class tcp_server::__internal_data final : public unix_socket
  {
  public:
    std::queue<int> fds;
    std::mutex fds_mutex;

  public:
    std::thread create_thread() final;
    void cancel(std::shared_ptr<bool> cancel) final;

    std::optional<std::unique_ptr<tcp_socket>>
    accept(std::shared_ptr<bool> cancel);

  public:
    ~__internal_data();
  };

  class udp_socket::__internal_data final : public unix_socket
  {
  public:
    std::thread create_thread() final;
    void cancel(std::shared_ptr<bool> cancel) final;
  };
} // namespace fscompsoc::net
