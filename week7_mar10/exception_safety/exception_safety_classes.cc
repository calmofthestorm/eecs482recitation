// This is not just a poor design in terms of clarity and performance, it is
// actually susceptible to memory leaks. Can you see why?

class SimpleBadDesign {
  std::queue<impl*>* some_queue;
  std::unordered_map<int, impl*>* some_map;
  int* some_int;
  bool* some_bool;

public:
  SimpleBadDesign();
  ~SimpleBadDesign();
};

SimpleBadDesign::SimpleBadDesign() {
  some_queue = new std::queue<impl*>;
  some_map = new std::unordered_map<int, impl*>;
  some_int = new int(3);
  some_bool = new bool(false);
}

// NOTE: It makes no difference if I write it like this instead:
SimpleBadDesign::SimpleBadDesign()
: some_queue(new std::queue<impl*>),
  some_map(new std::unordered_map<int, impl*>),
  some_int(new int(3)),
  some_bool(new bool(false))
{ }

SimpleBadDesign::~SimpleBadDesign() {
  delete some_queue;
  delete some_map;
  delete some_int;
  delete some_bool;
}

// ----------------------------------------------------------------------------

// This is not just a poor design in terms of clarity and performance, it is
// actually susceptible to segfaults. Can you see why?

class ReallyBadDesign {
  std::queue<impl*>* some_queue;
  std::unordered_map<int, impl*>* some_map;
  int* some_int;
  bool* some_bool;

public:
  void Init();

  ReallyBadDesign();
  ~ReallyBadDesign();
};

ReallyBadDesign::ReallyBadDesign()
{ }

ReallyBadDesign::~ReallyBadDesign() {
  delete some_queue;
  delete some_map;
  delete some_int;
  delete some_bool;
}

ReallyBadDesign::Init() {
  some_queue = new std::queue<impl*>;
  some_map = new std::unordered_map<int, impl*>;
  some_int = new int(3);
  some_bool = new bool(false);
}

// ----------------------------------------------------------------------------

// This is a poor design in terms of clarity and performance, but won't leak
// memory or lead to segfaults.

class CorrectBadDesign {
  std::queue<impl*>* some_queue;
  std::unordered_map<int, impl*>* some_map;
  int* some_int;
  bool* some_bool;

public:
  void Init();

  CorrectBadDesign();
  ~CorrectBadDesign();
};

CorrectBadDesign::CorrectBadDesign()
: some_queue(nullptr),
  some_map(nullptr),
  some_int(nullptr),
  some_bool(nullptr)
{ }

CorrectBadDesign::~CorrectBadDesign() {
  delete some_queue;
  delete some_map;
  delete some_int;
  delete some_bool;
}

CorrectBadDesign::Init() {
  some_queue = new std::queue<impl*>;
  some_map = new std::unordered_map<int, impl*>;
  some_int = new int(3);
  some_bool = new bool(false);
}

// ----------------------------------------------------------------------------

// This is a poor design in terms of clarity and performance, and can segfault.

class YetAnotherIncorrectBadDesign {
  std::queue<impl*>* some_queue;
  std::unordered_map<int, impl*>* some_map;
  int* some_int;
  bool* some_bool;

public:
  YetAnotherIncorrectBadDesign();
  ~YetAnotherIncorrectBadDesign();
};

YetAnotherIncorrectBadDesign::YetAnotherIncorrectBadDesign() {
  try {
    some_queue = new std::queue<impl*>;
    some_map = new std::unordered_map<int, impl*>;
    some_int = new int(3);
    some_bool = new bool(false);
  } catch (...) {
    delete some_queue;
    delete some_map;
    delete some_int;
    delete some_bool;
    throw;
  }
}

YetAnotherIncorrectBadDesign::~YetAnotherIncorrectBadDesign() {
  delete some_queue;
  delete some_map;
  delete some_int;
  delete some_bool;
}

// ----------------------------------------------------------------------------

// This is a poor design in terms of clarity and performance, and can leak
// memory.

class AnotherIncorrectBadDesign {
  std::queue<impl*>* some_queue;
  std::unordered_map<int, impl*>* some_map;
  int* some_int;
  bool* some_bool;

public:
  AnotherIncorrectBadDesign();
  ~AnotherIncorrectBadDesign();
};

AnotherIncorrectBadDesign::AnotherIncorrectBadDesign()
: some_queue(nullptr),
  some_map(nullptr),
  some_int(nullptr),
  some_bool(nullptr)
{
  some_queue = new std::queue<impl*>;
  some_map = new std::unordered_map<int, impl*>;
  some_int = new int(3);
  some_bool = new bool(false);
}

AnotherIncorrectBadDesign::~AnotherIncorrectBadDesign() {
  delete some_queue;
  delete some_map;
  delete some_int;
  delete some_bool;
}

// ----------------------------------------------------------------------------

// This is a poor design in terms of clarity (though one of the most clear so
// far) and performance, but won't leak memory or lead to segfaults.

class YetAnotherCorrectBadDesign {
  std::queue<impl*>* some_queue;
  std::unordered_map<int, impl*>* some_map;
  int* some_int;
  bool* some_bool;

public:
  YetAnotherCorrectBadDesign();
  ~YetAnotherCorrectBadDesign();
};

YetAnotherCorrectBadDesign::YetAnotherCorrectBadDesign()
: some_queue(nullptr),
  some_map(nullptr),
  some_int(nullptr),
  some_bool(nullptr)
{
  try {
    some_queue = new std::queue<impl*>;
    some_map = new std::unordered_map<int, impl*>;
    some_int = new int(3);
    some_bool = new bool(false);
  } catch(...) {
    delete some_queue;
    delete some_map;
    delete some_int;
    delete some_bool;
    throw;
  }
}

YetAnotherCorrectBadDesign::~YetAnotherCorrectBadDesign() {
  delete some_queue;
  delete some_map;
  delete some_int;
  delete some_bool;
}

// ----------------------------------------------------------------------------

// Hybrid design -- not as good as the final two, but can be done without
// relying on unique_ptrs anywhere except the constructor, which may be
// beneficial for working with legacy code.

class HybridDesign {
  std::queue<impl*>* some_queue;
  std::unordered_map<int, impl*>* some_map;
  int* some_int;
  bool* some_bool;

public:
  HybridDesign();
  ~HybridDesign();
};

HybridDesign::HybridDesign() {
  // Try to get memory
  std::unique_ptr<std::queue<impl*>> my_queue(new std::queue<impl*>);
  std::unique_ptr<std::unordered_map<int, impl*>> my_map (new std::unordered_map<int, impl*>);
  std::unique_ptr<int> my_int (new int(3));
  std::unique_ptr<bool> my_bool(new bool(false));

  // All allocation succeeded. Move memory into class.
  some_queue = my_queue.release();
  some_map = my_map.release();
  some_int = my_int.release();
  some_bool = my_bool.release();
}

HybridDesign::~HybridDesign() {
  delete some_queue;
  delete some_map;
  delete some_int;
  delete some_bool;
}

// ----------------------------------------------------------------------------

// Good designs...notice how SHORT AND SIMPLE they are? :-)

// Best Design #1
// This has the best performance and is tied for best clarity.

class GoodDesign1 {
  std::queue<impl*> some_queue;
  std::unordered_map<int, impl*> some_map;
  int some_int;
  bool some_bool;
};

// Best Design #2 -- slight performance and clarity hit over #1, but may be
// necessary in some cases where class hierarchies and inheritance are in use
// and you need dynamic dispatch (virtual calls).

class GoodDesign2 {
  std::unique_ptr<std::queue<impl*>> some_queue;
  std::unique_ptr<std::unordered_map<int, impl*>> some_map;
  std::unique_ptr<int> some_int;
  std::unique_ptr<bool> some_bool;
};

// Take home message:
// 0) Don't use a pointer to an X when a regular declaration will do (eg
//    GoodDesign1 > GoodDesign2, all else being equal).
// 1) Use unique_ptrs.
// 2) Use unique_ptrs.
// 3) Whenever you wish C++ had a finally, use RAII (like a unique_ptr).
// 4) Use RAII.
// 
// If you MUST break 1-4 for some reason, remember:
//
// 5) Remember that if a constructor throws an exception, the destructor is not
//    called. You need to clean up in the constructor.
// 6) Remember that exceptions thrown by objects initialized in constructor
//    lists can throw too, and you need that weird try / catch syntax to
//    catch them.
