CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-std=c++11 -g -gstabs -ggdb -I. -Wall -pedantic -Wextra
LDFLAGS=-std=c++11
LDLIBS=
MAKEFLAGS=-j 8

SRCS=best_infinite_loop_ever.cc  simple_context_example.cc  who_needs_for_loops.cc
OBJS=$(subst .cc,.o,$(SRCS))
EXES=$(subst .cc,,$(SRCS))

default:
	g++ $(LDFLAGS) -o best_infinite_loop_ever $(LDLIBS) best_infinite_loop_ever.cc
	g++ $(LDFLAGS) -o simple_context_example $(LDLIBS) simple_context_example.cc
	g++ $(LDFLAGS) -o who_needs_for_loops $(LDLIBS) who_needs_for_loops.cc

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS) $(EXES)

dist-clean: clean
	$(RM) *~ .dependtool
