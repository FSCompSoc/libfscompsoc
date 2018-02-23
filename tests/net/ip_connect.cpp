#include "fscompsoc/net/socket.hpp"
#include "fscompsoc/net/ip.hpp"

using namespace fscompsoc::net;
using namespace std;

int main(int argc, char const *argv[]) {
  ip_address addr("127.0.0.1");
  ip_endpoint ep(addr, 10000);
  tcp_socket sock(ep);

  vector<uint8_t> msg;

  msg.push_back('h');
  msg.push_back('i');
  msg.push_back(0);

  sock.send(msg);
  return 0;
}
