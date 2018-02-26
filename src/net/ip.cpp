#include "fscompsoc/net/socket.hpp"

#include "fscompsoc/exceptions.hpp"

using namespace std;
using namespace fscompsoc::exceptions;

namespace fscompsoc::net
{
  ip_address::ip_address(string str) : version(0)
  {
    auto start = str.begin();
    auto iter  = str.begin();

    while (iter != str.end()) {
      if (*iter == '.') {
        version = 4;
        break;
      } else if (*iter == ':') {
        version = 6;
        break;
      }
      iter++;
    }

    switch (version) {
    case 4: {
      bytes.reserve(4);
      bytes.push_back(stoi(string(start, iter)));
      for (int i = 0; i < 3; ++i) {
        start = ++iter;
        for (; *iter != '.' && iter != str.end(); ++iter)
          ;
        size_t dist = (iter - start);
        int val;
        if (dist == 0 || dist > 4 || (val = stoi(string(start, iter))) > 0xFF)
          throw InvalidArgument("An IPv4 group was out of range");

        bytes.push_back(val);
      }

      if (bytes.size() != 4)
        throw InvalidArgument("The IPv4 address was too short");
    } break;
    case 6: {
      if (str == "::") {
        bytes.assign(0, 8);
      } else {
        bytes.resize(16);

        vector<string> groups;

        while (iter != str.end()) {
          start = iter++;
          iter  = start;
          for (; *iter == ':' && iter != str.end(); iter++)
            ;
          groups.emplace_back(start, iter - 1);
        }

        if (groups.size() > 8)
          throw InvalidArgument(
              "The IPv6 address contained more than 8 groups");

        bool compressed = false;
        auto pos        = bytes.begin();

        for (const string& group : groups) {
          if (pos == bytes.end())
            throw InvalidArgument("The IPv6 address was too short");

          if (group.empty()) {
            if (compressed)
              throw InvalidArgument(
                  "The IPv6 address contained multiple \"::\"");

            compressed = true;
            for (size_t i = groups.size(); i < 8; ++i) {
              *pos++ = 0;
              *pos++ = 0;
            }
          } else {
            // Since it is in hex, we do not have to any fiddly checks
            if (groups.size() > 4)
              throw InvalidArgument("An IPv6 group was out of range");

            uint16_t val = stoi(string(start, iter - 1), nullptr, 16);
            // Put it in network byte order (big-endian)
            uint8_t data[2] = {static_cast<uint8_t>(val),
                               static_cast<uint8_t>(val >> 8)};

            // Shove it in the vector
            *pos++ = data[0];
            *pos++ = data[1];
          }
        }
      }
    } break;

    default:
      throw InternalException(ParseFailed("Invalid version type"));
    }
  }
} // namespace fscompsoc::net
