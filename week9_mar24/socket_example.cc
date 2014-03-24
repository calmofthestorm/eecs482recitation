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

class SocketExampleException {
  public:
    SocketExampleException(const std::string& err);
    std::string message;
};

SocketExampleException::SocketExampleException(const std::string& err)
: message(err) { }

class SocketHolder {
  public:
    SocketHolder(int fd);
    ~SocketHolder();
    int fd;
};

SocketHolder::SocketHolder(int fd_i)
: fd(fd_i) {
  if (fd < 0) {
    throw SocketExampleException(strerror(errno));
  }
}

SocketHolder::~SocketHolder() {
  close(fd);
}

class AddrinfoHolder {
  public:
    AddrinfoHolder(const std::string& host, const std::string& port, bool is_server);
    ~AddrinfoHolder();
    addrinfo* addr;
};

AddrinfoHolder::AddrinfoHolder(
    const std::string& host,
    const std::string& port,
    bool is_server) {

  // Set up the address
  addrinfo hints;
  memset(&hints, 0, sizeof(addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  if (is_server) {
    hints.ai_flags = AI_PASSIVE;
  }
  int rc = getaddrinfo(host.c_str(), port.c_str(), &hints, &addr);
  if (rc != 0) {
    throw SocketExampleException(gai_strerror(rc));
  }
}

AddrinfoHolder::~AddrinfoHolder() {
  freeaddrinfo(addr);
}

void server(const AddrinfoHolder& address) {
  // Allocate a socket
  SocketHolder master_sock(socket(AF_INET, SOCK_STREAM, 0));

  // Enable reuse -- you want this.
  int yes = 1;
  if (setsockopt(master_sock.fd, SOL_SOCKET,  SO_REUSEADDR, &yes, sizeof(int))) {
    throw SocketExampleException(strerror(errno));
  }

  // Bind to the interface.
  if (bind(master_sock.fd, address.addr->ai_addr, address.addr->ai_addrlen)) {
    throw SocketExampleException(strerror(errno));
  }

  // Listen.
  if (listen(master_sock.fd, 10)) {
    throw SocketExampleException(strerror(errno));
  }

  // Get socket port information
  sockaddr_in addr_name;
  socklen_t addr_len(sizeof(addr_name));
  if (getsockname(master_sock.fd, (sockaddr*)&addr_name, &addr_len)) {
    throw SocketExampleException(strerror(errno));
  }

  std::cout << "Server listening on port " << ntohs(addr_name.sin_port) << std::endl;

  while (1) {
    // Accept a connection.
    SocketHolder sock(accept(master_sock.fd, nullptr, 0));

    // Read message length.
    int length;
    if (recv(sock.fd, &length, sizeof(int), 0) != sizeof(int)) {
      throw SocketExampleException(strerror(errno));
    };

    // Read message.
    std::unique_ptr<char[]> buf(new char[length]);
    int count = 0;
    while (count < length) {
      int res = recv(sock.fd, &buf[count], length - count, 0);
      if (res <= 0) {
        throw SocketExampleException(strerror(errno));
      }
      count += res;
    }

    // Print message
    std::cout << std::string(buf.get(), length) << std::endl;

    // Send confirmation
    if (send(sock.fd, &count, sizeof(int), 0) != sizeof(int)) {
      throw SocketExampleException(strerror(errno));
    }
  }
}

void client(const AddrinfoHolder& address) {
  // Allocate a socket
  SocketHolder sock(socket(AF_INET, SOCK_STREAM, 0));

  // Connect.
  if (connect(sock.fd, address.addr->ai_addr, address.addr->ai_addrlen) != 0) {
    throw SocketExampleException(strerror(errno));
  }

  // Read message line by line
  std::string buf, tmp;
  while (getline(std::cin, tmp)) {
    buf += tmp;
  }

  // Send message length
  int length = (int)buf.size();
  if (send(sock.fd, &length, sizeof(int), 0) != sizeof(int)) {
    throw SocketExampleException(strerror(errno));
  };

  // Send message
  int count = 0;
  while (count < length) {
    int res = write(sock.fd, &buf.c_str()[count], length - count);
    if (res > 0) {
      count += res;
    } else {
      throw SocketExampleException(strerror(errno));
    }
  }

  // Receive and validate response
  if (recv(sock.fd, &length, sizeof(int), 0) != sizeof(int)) {
    throw SocketExampleException(strerror(errno));
  }

  if (length != count) {
    throw SocketExampleException("Server reports wrong number of bytes.");
  }
}

bool run(int argc, char** argv) {
  try {
    if (argc < 4) {
      return false;
    }

    std::string command(argv[1]);
    std::string hostname(argv[2]);
    std::string port(argv[3]);

    if (command == "client") {
      client(AddrinfoHolder(hostname, port, false));
    } else if (command == "server") {
      server(AddrinfoHolder(hostname, port, true));
    } else {
      return false;
    }

    return true;
  } catch(const SocketExampleException& er) {
    // Necessary to prevent the "bind address failed" error thingimie in
    // case of uncaught exception.
    std::cerr << er.message << std::endl;
    throw;
  }
}

int main(int argc, char** argv) {
  if (!run(argc, argv)) {
    std::cout << "Usage: " << argv[0] << " client|server hostname port"
              << std::endl;
  }
  return 0;
}
