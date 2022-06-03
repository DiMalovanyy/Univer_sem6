
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "mpi.h"

int* convert(char* c, int array_size) {

	int* array = malloc(sizeof(int)*array_size);
	int num, i = 0, len;
    while ( sscanf( c, "%d%n", &num, &len) == 1 ) {
        c += len;
        array[i++] = num;
    }
	return array;
}

int child_array[10000];

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

	int pid, np,
        elements_per_process,
        n_elements_recieved;

	 MPI_Status status;
	 MPI_Init(&argc, &argv);

     MPI_Comm_rank(MPI_COMM_WORLD, &pid);
     MPI_Comm_size(MPI_COMM_WORLD, &np);

	 if (pid == 0) {
        int index, i;
        elements_per_process = ARRAY_SIZE / np;

        if (np > 1) {
            for (i = 1; i < np - 1; i++) {
                index = i * elements_per_process;

                MPI_Send(&elements_per_process,
                         1, MPI_INT, i, 0,
                         MPI_COMM_WORLD);
                MPI_Send(&array[index],
                         elements_per_process,
                         MPI_INT, i, 0,
                         MPI_COMM_WORLD);
            }

            index = i * elements_per_process;
            int elements_left = ARRAY_SIZE - index;

            MPI_Send(&elements_left,
                     1, MPI_INT,
                     i, 0,
                     MPI_COMM_WORLD);
            MPI_Send(&array[index],
                     elements_left,
                     MPI_INT, i, 0,
                     MPI_COMM_WORLD);
        }

        int sum = 0;
        for (i = 0; i < elements_per_process; i++)
            sum += array[i];

        int tmp;
        for (i = 1; i < np; i++) {
            MPI_Recv(&tmp, 1, MPI_INT,
                     MPI_ANY_SOURCE, 0,
                     MPI_COMM_WORLD,
                     &status);
            int sender = status.MPI_SOURCE;

            sum += tmp;
        }

		printf("%i\n", sum);
    }
    else {
        MPI_Recv(&n_elements_recieved,
                 1, MPI_INT, 0, 0,
                 MPI_COMM_WORLD,
                 &status);

        MPI_Recv(&child_array, n_elements_recieved,
                 MPI_INT, 0, 0,
                 MPI_COMM_WORLD,
                 &status);

        int partial_sum = 0;
        for (int i = 0; i < n_elements_recieved; i++)
            partial_sum += child_array[i];

        MPI_Send(&partial_sum, 1, MPI_INT,
                 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
	free(array);
    exit(EXIT_SUCCESS);
}
