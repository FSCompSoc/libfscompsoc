#pragma once

#include "fscompsoc/net/ip.hpp"
#include "fscompsoc/async/attempt.hpp"

#include <functional>
#include <exception>
#include <memory>
#include <type_traits>
#include <string>

namespace fscompsoc::net {
  class socket {
  public:
    virtual async::maybe<std::vector<uint8_t>> receive() = 0;
    virtual async::attempt send(std::vector<uint8_t> data) = 0;
  };

  template<typename SocketType>
  class socket_server {
    static_assert(std::is_base_of_v<socket, SocketType>);

  public:
    virtual async::attempt start() = 0;
    virtual async::attempt stop() = 0;
    virtual async::maybe<std::unique_ptr<SocketType>> accept() = 0;
  };

  class bindable {
  public:
    virtual async::attempt bind() = 0;
  };

  using any_socket_server = socket_server<socket>;

  class tcp_socket : public socket {
  private:
    class __internal_data;
    __internal_data* __internal;

  public:
    async::maybe<std::vector<uint8_t>> receive() override;
    async::attempt send(std::vector<uint8_t> data) override;

  public:
    tcp_socket(ip_endpoint server);

    ~tcp_socket();
  };

  class tcp_server : public bindable, public socket_server<tcp_socket> {
  private:
    class __internal_data;
    __internal_data* __internal;

  public:
    async::maybe<std::unique_ptr<tcp_socket>> accept() override;
    async::attempt bind() override;
  };

  class udp_socket : public bindable, public socket {
  private:
    class __internal_data;
    __internal_data* __internal;

  public:
    async::maybe<std::vector<uint8_t>> receive() override;
    async::attempt send(std::vector<uint8_t> data) override;
    async::attempt bind() override;

  public:
    udp_socket(ip_endpoint server);

    ~udp_socket();
  };

  class udp_server : public bindable, public socket_server<udp_socket> {
  private:
    class __internal_data;
    __internal_data* __internal;

  public:
    async::maybe<std::unique_ptr<udp_socket>> accept() override;
    async::attempt bind() override;
  };
}
