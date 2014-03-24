#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>

addrinfo* get_addrinfo(
    const std::string& host,
    const std::string& port,
    bool is_server) {

  // Set up the address
  addrinfo hints, *addr;
  memset(&hints, 0, sizeof(addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  if (is_server) {
    hints.ai_flags = AI_PASSIVE;
  }
  assert(getaddrinfo(host.c_str(), port.c_str(), &hints, &addr) == 0);

  return addr;
}

void server(addrinfo* addr) {
  // Allocate a socket
  int master_sock(socket(AF_INET, SOCK_STREAM, 0));
  assert(master_sock > 0);

  // Enable reuse -- you want this.
  int yes = 1;
  assert(!setsockopt(master_sock, SOL_SOCKET,  SO_REUSEADDR, &yes, sizeof(int)));

  // Bind to the interface.
  assert(!bind(master_sock, addr->ai_addr, addr->ai_addrlen));

  // Listen.
  assert(!listen(master_sock, 10));

  // Get socket port information
  sockaddr_in addr_name;
  socklen_t addr_len(sizeof(addr_name));
  assert(!getsockname(master_sock, (sockaddr*)&addr_name, &addr_len));

  std::cout << "Server listening on port " << ntohs(addr_name.sin_port) << std::endl;

  while (1) {
    // Accept a connection.
    int sock(accept(master_sock, nullptr, 0));
    assert(sock > 0);

    // Read message length.
    int length;
    assert(recv(sock, &length, sizeof(int), 0) == sizeof(int));

    // Read message.
    char* buf = new char[length];
    int count = 0;
    while (count < length) {
      int res = recv(sock, &buf[count], length - count, 0);
      assert(res > 0);
      count += res;
    }

    // Print message
    std::cout << std::string(buf, length) << std::endl;

    // Send confirmation
    assert(send(sock, &count, sizeof(int), 0) == sizeof(int));
  }
}

void client(addrinfo* addr) {
  // Allocate a socket
  int sock(socket(AF_INET, SOCK_STREAM, 0));
  assert(sock > 0);

  // Connect.
  assert(!connect(sock, addr->ai_addr, addr->ai_addrlen));

  // Read message line by line
  std::string buf, tmp;
  while (getline(std::cin, tmp)) {
    buf += tmp;
  }

  // Send message length
  int length = (int)buf.size();
  assert(send(sock, &length, sizeof(int), 0) == sizeof(int));

  // Send message
  int count = 0;
  while (count < length) {
    int res = write(sock, &buf.c_str()[count], length - count);
    assert(res > 0);
    count += res;
  }

  // Receive and validate response
  assert(recv(sock, &length, sizeof(int), 0) == sizeof(int));
  assert(length == count);
}

bool run(int argc, char** argv) {
  if (argc < 4) {
    return false;
  }

  std::string command(argv[1]);
  std::string hostname(argv[2]);
  std::string port(argv[3]);

  addrinfo* addr = get_addrinfo(hostname, port, command == "server");
  assert(addr);

  if (command == "client") {
    client(addr);
  } else if (command == "server") {
    server(addr);
  } else {
    return false;
  }

  return true;
}

int main(int argc, char** argv) {
  if (!run(argc, argv)) {
    std::cout << "Usage: " << argv[0] << " client|server hostname port"
              << std::endl;
  }
  return 0;
}
