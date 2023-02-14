CC = g++
CFLAGS = -Wall -Wextra -O2
LDLIBS = -lm

build: ff_test.o pr_test.o mkm_test.o

run-best:
	./test3

run-p1:
	./test1

run-p2:
	./test2

run-p3:
	./test3

ff.o: ff.cpp
	g++ ff.cpp -c $(CFLAGS)

ff_test.o: test.cpp ff.o
	g++ ff.o test.cpp -o test1 $(CFLAGS)

pr.o: pr.cpp
	g++ pr.cpp -c $(CFLAGS)

pr_test.o: test.cpp pr.o
	g++ pr.o test.cpp -o test2 $(CFLAGS)

mkm.o: mkm.cpp
	g++ mkm.cpp -c $(CFLAGS)

mkm_test.o: test.cpp mkm.o
	g++ mkm.o test.cpp -o test3 $(CFLAGS)

.PHONY: clean

clean:
	rm -rf *.o test1 test2 test3 test.in test.out
