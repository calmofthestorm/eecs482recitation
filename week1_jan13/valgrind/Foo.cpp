#include "Foo.h"

// We need to include FooImpl.h so the compiler
// knows what functions can be called on a FooImpl.
#include "FooImpl.h"

Foo::Foo() : impl_(new Foo::FooImpl())
{
}

Foo::~Foo()
{
	// Memory leak! Oh noes!
	// delete impl_;
}

void Foo::bar()
{
	impl_->bar();
}
