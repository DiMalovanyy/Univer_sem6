
all: lab_1 lab_2 lab_3

lab_1: lab_1.c
	gcc -o $@ $^

lab_2: lab_2.c
	mpicc -o $@ $^

lab_3: lab_3.c
	gcc -fopenmp -o $@ $^

.PHONY: clean
clean:
	rm -rf lab_1
	rm -rf lab_2
	rm -rf lab_3
	rm -rf *.o
	rm -rf *.dSYM
