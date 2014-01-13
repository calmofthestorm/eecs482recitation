#include "HelloWorld.h"

#include <iostream>

struct HelloWorld::HelloWorldImpl {
  int greeting_count;
};

HelloWorld::HelloWorld()
: pImpl(new HelloWorldImpl) { }

HelloWorld::~HelloWorld() = default;

void HelloWorld::say_hello() const {
  std::cout << "I have greeted you " << pImpl->greeting_count++ << " times. "
            << "Hello, by the way." << std::endl;
}

void HelloWorld::say_goodbye() const {
  std::cout << "Goodbye!" << std::endl;
}
