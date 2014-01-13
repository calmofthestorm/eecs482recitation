#include <iostream>
using std::cout;
using std::endl;

class StaticTest
{
public:
	StaticTest() : instanceCalls(0) {}

	static void foo()
	{
		cout << "Called " << ++totalCalls << " times" << endl;

		/*
		// ERROR to refer to instance members directly.
		cout << "Called " << ++instanceCalls << " times on this instance." << endl;
		*/
	}

	static void bar(StaticTest *x)
	{
		cout << "Called " << ++totalCalls << " times" << endl;

		cout << "Called " << ++x->instanceCalls << " times on instance" << endl;
	}

private:
	static int totalCalls;
	int instanceCalls;
};

// Define and initialize static data member.
int StaticTest::totalCalls = 0;

int main()
{
	StaticTest::foo();
	StaticTest::foo();
	StaticTest::foo();

	StaticTest inst1, inst2;
	StaticTest::bar(&inst1);
	StaticTest::bar(&inst2);

	// Works but considered poor style.
	inst1.foo();
}

