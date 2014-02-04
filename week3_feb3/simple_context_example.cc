#include <atomic>
#include <iostream>
#include <cassert>
#include <stdexcept>
#include <ucontext.h>

const size_t STACK_SIZE = 262144;

ucontext_t c1, c2;

void foo(const char* str1, int count) {
  for (size_t i = 0; i < count; ++i) {
    std::cout << str1 << std::endl;
  }
  setcontext(&c2);
}

int main() {
  getcontext(&c1);
  getcontext(&c2);

  c1.uc_stack.ss_sp = new char[STACK_SIZE];
  c1.uc_stack.ss_size = STACK_SIZE;
  c1.uc_stack.ss_flags = 0;
  c1.uc_link = nullptr;

  // Since we are going to store the context of the program when it was started
  // into c2, we don't need to allocate it a stack -- Essentially we are simply
  // saving the state of the main program into c2 while c1 runs.

  makecontext(&c1, (void(*)())foo, 2, "hello world", 3);
  swapcontext(&c2, &c1);
  std::cout << "goodbye world" << std::endl;

  delete[] (char**)c1.uc_stack.ss_sp;

  return 0;
}
