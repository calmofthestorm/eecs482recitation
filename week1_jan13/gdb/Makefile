demo: demo.o thread.o libcpu.a
	g++ -ldl -pthread demo.o thread.o libcpu.a -o demo

demo.o: demo.cpp cpu.h cv.h mutex.h thread.h
	g++ -g -c demo.cpp -o demo.o -std=c++11

clean:
	rm -f demo.o demo
