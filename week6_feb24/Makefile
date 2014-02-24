CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-std=c++11 -g -gstabs -ggdb -I. -Wall -pedantic -Wextra
LDFLAGS=-std=c++11
LDLIBS=
MAKEFLAGS=-j 8

default:
	g++ $(LDFLAGS) -o scheduling $(LDLIBS) scheduling.cc

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS) $(EXES)

dist-clean: clean
	$(RM) *~ .dependtool
