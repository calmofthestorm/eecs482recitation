#ifndef FOO_H
#define FOO_H

class Foo
{
public:
	Foo();
	~Foo();
	void bar();

private:
	// This tells the compiler that class FooImpl
	// exists and is defined *somewhere*.
	// We can't use FooImpl directly, but we can make pointers
	// or references to FooImpl.
	class FooImpl;

	FooImpl *impl_; // The actual pImpl.
};

#endif
