#include "net/socket.hpp"

#include "exceptions.hpp"

using namespace std;
using namespace fscompsoc::exceptions;

namespace fscompsoc::net {
  ip_address::ip_address(string str) : version(0) {
    auto start = str.begin();
    auto iter = str.begin();

    while (*iter != str.end()) {
      if (*iter == '.') {
        version == 4;
        break;
      } else if (*iter == ':') {
        version == 6;
        break;
      }
      iter++;
    }

    switch (version) {
      case(4):

      default:
        throw(InternalException(ParseFailed("Invalid version type")));
    }
  }
}
