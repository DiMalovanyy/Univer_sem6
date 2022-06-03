
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "mpi.h"
#include "errno.h"

int* convert(char* c, int array_size) {
	int* array = malloc(sizeof(int)*array_size);
	int num, i = 0, len;
    while ( sscanf( c, "%d%n", &num, &len) == 1 ) {
        c += len;
        array[i++] = num;
    }
	return array;
}

int main(int argc, char ** argv) {
	errno = 0;

	if (argc == 2) {
		printf("Usage: %s '<elem1> <elem2> ...' <elems_size>\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	int ARRAY_SIZE = strtol(argv[2], NULL, 10);
	if (errno != 0) {
		printf("Size %s should be integer type\n", argv[2]);
		exit(EXIT_FAILURE);
	}
	int* array = convert(argv[1], ARRAY_SIZE);



	free(array);

	exit(EXIT_SUCCESS);
}
