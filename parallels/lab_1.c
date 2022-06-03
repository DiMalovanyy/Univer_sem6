#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

int accumulate(int* array, int size) {
	int accumulator = 0;

	for (int i = 0; i < size; i++) {
		accumulator += array[i];
	}
	return accumulator;
}


int main(int argc, char ** argv) {
	errno = 0;
	int ARRAY_SIZE = argc - 1;

	int array[ARRAY_SIZE];
	if (argc == 1) {
		printf("Usage: %s <elem1> <elem2> ...\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	for (int i = 1; i < argc; i++) {
		array[i - 1] = strtol(argv[i], NULL, 10);
		if (errno != 0) {
			printf("Provide elements as integers.\n");
			exit(EXIT_FAILURE);
		}
	}

	int result = accumulate(array, ARRAY_SIZE);
	printf("%i\n", result);

	exit(EXIT_SUCCESS);
}
