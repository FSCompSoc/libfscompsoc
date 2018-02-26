#pragma once

#include "fscompsoc/async/action.hpp"
#include "fscompsoc/async/attempt.hpp"
#include "fscompsoc/net/ip.hpp"

#include <exception>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>

namespace fscompsoc::net
{
  class socket
  {
  public:
    FSCOMPSOC_MAKE_EXCEPTION(ConnectionFailed, "The connection failed")
  public:
    virtual async::action<std::vector<uint8_t>> receive()  = 0;
    virtual async::attempt send(std::vector<uint8_t> data) = 0;
  };

  template <typename SocketType>
  class socket_server
  {
    static_assert(std::is_base_of_v<socket, SocketType>);

  public:
    virtual async::attempt start()                              = 0;
    virtual async::attempt stop()                               = 0;
    virtual async::action<std::unique_ptr<SocketType>> accept() = 0;
  };

  class bindable
  {
  public:
    virtual async::attempt bind() = 0;
  };

  using any_socket_server = socket_server<socket>;

  // Be aware that TCP is effectively stream of bytes, with no discrete
  // 'messages'
  class tcp_socket : public socket
  {
  private:
    class __internal_data;
    __internal_data* __internal;

  public:
    async::action<std::vector<uint8_t>> receive() override;
    async::attempt send(std::vector<uint8_t> data) override;

  public:
    tcp_socket(ip_endpoint server);

    ~tcp_socket();

  private:
    tcp_socket(__internal_data*);
  };

  class tcp_server : public bindable, public socket_server<tcp_socket>
  {
  private:
    class __internal_data;
    __internal_data* __internal;

  public:
    async::action<std::unique_ptr<tcp_socket>> accept() override;
    async::attempt bind() override;

  public:
    tcp_server();

    ~tcp_server();

  private:
    tcp_server(__internal_data*);
  };

  class udp_socket : public bindable, public socket
  {
  private:
    class __internal_data;
    __internal_data* __internal;

  public:
    async::action<std::vector<uint8_t>> receive() override;
    async::attempt send(std::vector<uint8_t> data) override;
    async::attempt bind() override;

  public:
    udp_socket();

    ~udp_socket();

  private:
    udp_socket(__internal_data*);
  };
} // namespace fscompsoc::net
