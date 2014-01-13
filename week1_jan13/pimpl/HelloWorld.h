#include <memory>

class HelloWorld {
  public:
    HelloWorld();

    // necessary to use a unique pointer. If this is missing, then the
    // default dtor will be generated in the header file, which can't
    // see the actual type's sizeof, and thus can't create code.
    ~HelloWorld();

    void say_hello() const;
    void say_goodbye() const;

  private:
    struct HelloWorldImpl;
    std::unique_ptr<HelloWorldImpl> pImpl;
};
