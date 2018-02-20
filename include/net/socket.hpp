#pragma once

#include <functional>
#include <future>
#include <vector>
#include <cstdint>
#include <exception>
#include <memory>
#include <type_traits>
#include <string>

namespace fscompsoc::net {
  class socket {
  public:
    virtual std::future<std::vector<uint8_t>> receive();
    virtual std::future<bool> send(std::vector<uint8_t> data);
  };

  template<typename SocketType>
  class socket_server {
    static_assert(std::is_base_of_v<socket, SocketType>);

  public:
    virtual std::unique_ptr<SocketType> accept();
  };

  using any_socket_server = socket_server<socket>;

  class ip_address {
  public:
  public:
    int version;
    std::vector<uint8_t> bytes;

  public:
    ip_address(std::string);
    ip_address(int version, std::vector<uint8_t> bytes) :
      version(version), bytes(bytes) {};
  };

  class ip_endpoint {
  public:
    ip_address addr;
    uint16_t port;

  public:
    ip_endpoint(std::string);
    ip_endpoint(ip_address addr, uint16_t port) :
      addr(addr), port(port) {};
  };

  class tcp_socket : public socket {
  public:
    std::future<std::vector<uint8_t>> receive() override;
    std::future<bool> send(std::vector<uint8_t> data) override;

  public:
    tcp_socket(ip_endpoint server);

    ~tcp_socket();
  };

  class tcp_server : public socket_server<tcp_socket> {
  public:
    std::unique_ptr<tcp_socket> accept() override;
  };

  class udp_socket : public socket {
  public:
    std::future<std::vector<uint8_t>> receive() override;
    std::future<bool> send(std::vector<uint8_t> data) override;

  public:
    udp_socket(ip_endpoint server);

    ~udp_socket();
  };

  class udp_server : public socket_server<udp_socket> {
  public:
    std::unique_ptr<udp_socket> accept() override;
  };
}
