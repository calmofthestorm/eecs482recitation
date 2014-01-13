#include "Foo.h"
#include "FooImpl.h"
#include <iostream>
using std::cout;
using std::endl;

Foo::FooImpl::FooImpl()
{
	cout << "FooImpl constructed" << endl;
}

void Foo::FooImpl::bar()
{
	cout << "Hello, world!" << endl;
}
