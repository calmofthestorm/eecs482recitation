#include <iostream>
#include <cassert>

#include <ucontext.h>

const size_t STACK_SIZE = 262144;

// Context to store our position in the loop.
ucontext_t loop_context;

// Stores the start of the loop's body
ucontext_t body_context;

// Stores where to go when the loop is done
ucontext_t loop_done_context;

void for_loop(int count) {
  // Loop can't iterate 0 times because there is no way to skip the body.
  assert(count > 0);

  // First iteration we need to save in the body of the function. If we
  // returned to the context created my makecontext, count would be
  // reset each time. (this particular restriction is unlikely to come
  // up in the project)
  --count;
  swapcontext(&loop_context, &body_context);

  // If count is not done yet, run the body.
  if (count--) {
    setcontext(&body_context);
  }

  setcontext(&loop_done_context);
}

int main() {
  // You need to call getcontext before you call makecontext.
  getcontext(&loop_context);
  getcontext(&body_context);
  getcontext(&loop_done_context);

  // Set up the three context data structures.
  for (auto ctx : {&loop_context, &body_context, &loop_done_context}) {
    ctx->uc_stack.ss_sp = new char[STACK_SIZE];
    ctx->uc_stack.ss_size = STACK_SIZE;
    ctx->uc_stack.ss_flags = 0;
    ctx->uc_link = nullptr;
  }

  // When the loop is done, restore this context.
  loop_context.uc_link = nullptr;

  // Set the loop context so that when it is activated we call the function
  // for_loop with 1 argument, 10. fun fact: did you know that the ability of
  // makecontext to call functions with arbitrary number of arguments is why
  // make ucontext is deprecated in later POSIX?
  makecontext(&loop_context, (void(*)())for_loop, 1, 10);

  // Save our current context to body_context and activate loop_context.
  swapcontext(&body_context, &loop_context);

  // Begin the loop body.
  std::cout << "goodbye world" << std::endl;
  // End the loop body.

  // Save the current context to loop_done_context and activate loop_context.
  swapcontext(&loop_done_context, &loop_context);

  // Free the context stacks.
  for (const auto& ctx : {loop_context, body_context, loop_done_context}) {
    delete[] (char**)ctx.uc_stack.ss_sp;
  }

  return 0;
}
