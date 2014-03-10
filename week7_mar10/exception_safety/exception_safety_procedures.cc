// The easiest way to handle memory [de]allocation is to put it into a class and
// make use of constructors and destructors. In this file I will discuss how to
// get multiple chunks of memory safely when that's not possible.

// All of this example presumes we cannot alter my_struct or do_stuff.
struct my_struct {
  std::queue<impl*>* my_queue;
  char* my_stack[];
};

void do_stuff() {
  my_struct s;
  get_resources(&s);

  // ...

}

// The (wrong) C way (leaks memory)
// (for these C examples, note that using malloc to allocate STL structures
// isn't ok. Let's temporarily pretend that it's ok for the purposes of these
// examples only.
int get_resources(my_struct* s) {
  s->my_queue = (std::queue<impl*>*)malloc(sizeof(std::queue<impl*>));
  s->my_stack = (char**)malloc(sizeof(char[STACK_SIZE]));
  if (!s->my_queue || !s->my_stack) {
    return -1;
  } else {
    return 0;
  }
}

// The (correct) C way 1
int get_resources(my_struct* s) {
  s->my_queue = (std::queue<impl*>*)malloc(sizeof(std::queue<impl*>));
  s->my_stack = (char**)malloc(sizeof(char[STACK_SIZE]));
  if (!s->my_queue || !s->my_stack) {
    free(s->my_queue);
    free(s->my_stack);
    return -1;
  } else {
    return 0;
  }
}

// The (correct) C way 2 (less readable but avoids unnecessary calls to
// malloc.) Notice how poorly this scales to allocating many things.
int get_resources(my_struct* s) {
  s->my_queue = (std::queue<impl*>*)malloc(sizeof(std::queue<impl*>));
  if (s->my_queue) {
    s->my_stack = (char**)malloc(sizeof(char[STACK_SIZE]));
  }
  if (!s->my_queue || !s->my_stack) {
    free(s->my_queue);
    free(s->my_stack);
    return -1;
  } else {
    return 0;
  }
}

// BAD AND WRONG C++ way 1 -- can leak memory if second new throws.
void get_resources(my_struct* s) {
  s->my_queue = new std::queue<impl*>;
  s->my_stack = new char[STACK_SIZE];
}

// BAD AND WRONG C++ way 2 -- can segfault
void get_resources(my_struct* s) {
  try {
    s->my_queue = new std::queue<impl*>;
    s->my_stack = new char[STACK_SIZE];
  } catch (...) {
    delete s->my_queue;
    delete s->my_stack;
  }
}

// BAD BUT CORRECT C++ way 1
void get_resources(my_struct* s) {
  s->my_queue = nullptr;
  s->my_stack = nullptr;
  try {
    s->my_queue = new std::queue<impl*>;
    s->my_stack = new char[STACK_SIZE];
  } catch (...) {
    delete s->my_queue;
    delete s->my_stack;
  }
}

// GOOD AND CORRECT C++ way 1
void get_resources(my_struct* s) {
  // Get memory
  std::unique_ptr<std::queue<impl*>> the_queue(new std::queue<impl*>);
  std::unique_ptr<char[]> the_stack(new char[STACK_SIZE]);

  // Move memory out of smart pointers.
  s->my_queue = the_queue.release();
  s->my_stack = the_stack.release();
}

// BEST C++ way: use smart pointers in do_stuff and my_struct. [not shown]

// WHAT IS THIS I DON'T EVEN (but correct) C++ way
// (needs #include <new>)
int get_resources(my_struct* s) {
  s->my_queue = new (std::nothrow) std::queue<impl*>;
  s->my_stack = new (std::nothrow) char[STACK_SIZE];
  if (!s->my_queue || !s->my_stack) {
    free(s->my_queue);
    free(s->my_stack);
    return -1;
  } else {
    return 0;
  }
}

// (it is also possible to overload new operator so nothrow is default
// behavior. [not shown])
