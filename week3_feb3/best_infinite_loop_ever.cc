#include <cassert>
#include <iostream>
#include <ucontext.h>

const size_t STACK_SIZE = 262144;

ucontext_t i_can_see_forever;

int main() {
  // Create an std::initializer_list of words to shout.
  const auto words = {
      "Ph'nglui",
      "mglw'nafh",
      "Cthulhu",
      "R'lyeh",
      "wgah'nagl",
      "fhtagn",
    };

  auto it = words.begin();

  // This is where we come back to.
  getcontext(&i_can_see_forever);

  // Cycle words when we run out.
  if (it == words.end()) {
    it = words.begin();
  }

  // Cthulhu fhtagn!
  std::cout << "I can see forever! " << *it++ << std::endl;

  // Go back to just after getcontext.
  setcontext(&i_can_see_forever);
}
