#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace fscompsoc::net
{
  class ip_address
  {
  public:
  public:
    int version;
    std::vector<uint8_t> bytes;

  public:
    ip_address(std::string);
    ip_address(int version, std::vector<uint8_t> bytes)
        : version(version), bytes(bytes){};
  };

  class ip_endpoint
  {
  public:
    ip_address addr;
    uint16_t port;

  public:
    ip_endpoint(std::string);
    ip_endpoint(ip_address addr, uint16_t port) : addr(addr), port(port){};
  };
} // namespace fscompsoc::net
