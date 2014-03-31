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

// BEGIN
// Stuff from last week

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
  hints.ai_family = AF_INET;
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

// END
// Stuff from last week

struct Node {
  char *nodedata;
  Node *next;
};

struct List {
  size_t length;
  Node* head;
};

void free_list(List* list) {
  if (list) {
    Node* cur = list->head;
    delete list;
    while (cur) {
      Node* tmp = cur;
      cur = cur->next;
      delete[] tmp->nodedata;
      delete tmp;
    }
  }
}

class ListHolder {
  public:
    List* list;
    ListHolder()
      : list(nullptr) { }

    ListHolder(List* l)
      : list(l) { }

    ~ListHolder() {
      free_list(list);
    }
};

int sendData(int fd, const void* buf, size_t length) {
  size_t sent = 0;
  while (sent < length) {
    int status = send(fd, (void*)((uintptr_t)buf + sent), length - sent, 0);
    if (status <= 0) {
      return -1;
    }
    sent += status;
  }
  return 0;
}

int receiveData(int fd, void* buf, size_t length) {
  size_t got = 0;
  while (got < length) {
    int status = recv(fd, (void*)((uintptr_t)buf + got), length - got, 0);
    if (status <= 0) {
      return -1;
    }
    got += status;
  }
  return 0;
}

int send_list(const List* list, int fd) {
  // Send list length.
  if (sendData(fd, &list->length, sizeof(list->length))) {
    return -1;
  }

  // Send all elements.
  size_t length = list->length;
  Node* cur = list->head;
  while (length--) {
    size_t element_length = strlen(cur->nodedata) + 1;
    if (sendData(fd, &element_length, sizeof(element_length))) {
      return -1;
    }

    if (sendData(fd, cur->nodedata, element_length)) {
      return -1;
    }

    cur = cur->next;
  }

  return 0;
}

// C++ RAII receive_list. Readable and checks errors.
List* receive_list(int fd) {
  ListHolder lh(new List{0, nullptr});
  List* list = lh.list;

  // Receive list length.
  if (receiveData(fd, &list->length, sizeof(list->length))) {
    return nullptr;
  }

  if (list->length > 0) {
    // First element is special case.
    list->head = new Node{nullptr, nullptr};

    Node* cur = nullptr;

    // Read in the elements.
    while (list->length-- > 0) {
      if (!cur) {
        cur = list->head = new Node{nullptr, nullptr};
      } else {
        cur->next = new Node{nullptr, nullptr};
        cur = cur->next;
      }

      // Read element length
      size_t element_length;
      if (receiveData(fd, &element_length, sizeof(element_length))) {
        return nullptr;
      }

      // Read element.
      cur->nodedata = new char[element_length];
      if (receiveData(fd, cur->nodedata, element_length)) {
        return nullptr;
      }
    }
  }

  lh.list = nullptr;
  return list;
}

// Receive list (C-style) no error checking. Readable.
List* receive_list_c_noerrorchecking(int fd) {
  List* list = new (std::nothrow) List{0, nullptr};

  // Receive list length.
  assert(!receiveData(fd, &list->length, sizeof(list->length)));

  if (list->length > 0) {
    // First element is special case.
    assert((list->head = new (std::nothrow) Node{nullptr, nullptr}));

    Node* cur = nullptr;

    // Read in the elements.
    while (list->length-- > 0) {
      if (!cur) {
        cur = list->head = new (std::nothrow) Node{nullptr, nullptr};
      } else {
        cur->next = new (std::nothrow) Node{nullptr, nullptr};
        cur = cur->next;
      }

      // Read element length
      size_t element_length;
      assert(!receiveData(fd, &element_length, sizeof(element_length)));

      // Read element.
      assert(cur->nodedata = new (std::nothrow) char[element_length]);
      assert(!receiveData(fd, cur->nodedata, element_length));
    }
  }

  return list;
}

// Error-checked C-style receive_list. Checks errors.
List* receive_list_c(int fd) {
  List* list = new (std::nothrow) List{0, nullptr};
  if (!list) {
    return nullptr;
  }

  // Receive list length.
  if (receiveData(fd, &list->length, sizeof(list->length))) {
    goto fail;
  }

  if (list->length > 0) {
    // First element is special case.
    if (!(list->head = new (std::nothrow) Node{nullptr, nullptr})) {
      goto fail;
      goto fail;
    }

    Node* cur = nullptr;

    // Read in the elements.
    while (list->length-- > 0) {
      if (!cur) {
        cur = list->head = new (std::nothrow) Node{nullptr, nullptr};
      } else {
        cur->next = new (std::nothrow) Node{nullptr, nullptr};
        cur = cur->next;
      }

      // Read element length
      size_t element_length;
      if (receiveData(fd, &element_length, sizeof(element_length))) {
        goto fail;
      }

      // Read element.
      if (!(cur->nodedata = new (std::nothrow) char[element_length]) ||
          receiveData(fd, cur->nodedata, element_length)) {
        goto fail;
      }
    }
  }

  return list;

fail:
  free_list(list);
  return nullptr;
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

    // Receive a list.
    ListHolder list(receive_list(sock.fd));
    if (!list.list) {
      std::cout << "Failed to successfully receive list." << std::endl;
    } else {
      std::cout << "***" << std::endl;
      Node* cur = list.list->head;
      while (cur) {
        assert(cur && cur->nodedata);
        std::cout << cur->nodedata << std::endl;
        cur = cur->next;
      }
      std::cout << "***\n" << std::endl;
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

  // Allocate list.
  ListHolder list(new List{0, nullptr});

  // Read list, one element per line.
  Node* cur = nullptr;
  std::string tmp;
  while (getline(std::cin, tmp)) {
    ++list.list->length;
    // Allocate node
    if (list.list->head) {
      cur = cur->next = new Node{nullptr, nullptr};
    } else {
      cur = list.list->head = new Node{nullptr, nullptr};
    }

    // Allocate and copy string.
    strncpy(cur->nodedata = new char[tmp.size() + 1], tmp.c_str(), 1 + tmp.size());
  }

  // Send list.
  if (send_list(list.list, sock.fd)) {
    std::cout << "Error sending the list." << std::endl;
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
