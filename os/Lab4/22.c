#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>


typedef struct matrix {
	int rows;
	int columns;
	double** data;
} matrix_t;


matrix_t* create_matrix(int rows, int columns) {
	matrix_t* matrix = malloc(sizeof(matrix_t));
	matrix->data = malloc(sizeof(double)*rows);
	for (int i = 0; i < rows; i++) {
		matrix->data[i] = malloc(sizeof(double)*columns);
	}
	matrix->rows = rows;
	matrix->columns = columns;
	return matrix;
}

void delete_matrix(matrix_t* matrix) {
	if (matrix == NULL) {
		printf("Invalid matrix pointer\n");
		return;
	}

	for(int i = 0; i < matrix->rows; i++) {
		free(matrix->data[i]);
	}
	free(matrix->data);
	free(matrix);
}

void print_matrix(const matrix_t* matrix, FILE* out) {
	if (!matrix || !matrix->data) {
		fprintf(out, "Error\n");
		return;
	}

	fprintf(out,"Size [%d x %d]\n", matrix->rows, matrix->columns);
	for (int i = 0; i < matrix->rows; i++) {
		for (int j = 0; j < matrix->columns; j++) {
			fprintf(out, "%f ", matrix->data[i][j]);
		}
		fprintf(out, "\n");
	}
}

void print_row(const double* row, int row_size, FILE* out) { 
	fprintf(out, "[ ");
	for (int i = 0; i < row_size; i++) {
		fprintf(out, "%lf ", row[i]);
	}
	fprintf(out, "]\n");

}
/* Matrix file format
 * <rows> <columns>
 * x1 x2 x3 ...
 * x4 x5 x6 ...
 * x7 x8 x9 ...
 * ... ... ... 
 */
matrix_t* read_matrix_from_file(FILE* file) {
	int rc;
	int rows, columns;
	rc = fscanf(file, "%d %d\n", &rows, &columns);
	if (rc != 2) {
		printf("Failed to read columns and rows from file\n");
		return NULL;
	} 

	matrix_t* matrix = create_matrix(rows, columns);
	for (int i = 0; i < rows; i++) {
		for(int j = 0; j < columns; j++) {
			rc = fscanf(file, "%lf", &(matrix->data[i][j]));
			if (rc != 1) {
				printf("Failed to read num\n");
			}
		}
	}

	return matrix;
}


/* NOTE: This shit allocates memory */
double* get_column(const matrix_t* matrix, int column_index) {
	double* column = malloc(sizeof(double) * matrix->columns);
	for(int i = 0; i < matrix->rows; i++) {
		for(int j = 0; j < matrix->columns; j++) {
			if (j == column_index) {
				column[i] = matrix->data[i][j];
			}
		}
	}
	return column;
}

int vector_multiply(const double* fst, const double* snd, int vector_sizes) {

	int result = 0;
	for (int i =0; i < vector_sizes; i++) {
		result += fst[i] * snd[i];
	}
	return result;
}

typedef struct worker_data {
	matrix_t* reuslt_matrix_ptr;
	int cur_row_index;
	int cur_column_index;
	
	const double* fst_vector;
	const double* snd_vector;
	int vector_sizes;
} worker_data_t;

void* worker_routine(void* arg) {
	worker_data_t* data = (worker_data_t*)arg;
	double result = vector_multiply(data->fst_vector, data->snd_vector, data->vector_sizes);
	data->reuslt_matrix_ptr->data[data->cur_row_index][data->cur_column_index] =  result;
	printf("Worker for [%d][%d] reult matrix finished work. Result: %lf\n", data->cur_row_index, data->cur_column_index, result);
	return NULL;
}

typedef struct worker {
	pthread_t worker_thread;
	worker_data_t* worker_data;
} worker_t;

matrix_t* multiply_matrix(const matrix_t* lhs, const matrix_t* rhs) {
	matrix_t* result_matrix = create_matrix(lhs->rows, rhs->columns);
	int operations_amount = lhs->rows * rhs-> columns;
	int rc;
	printf("Result mutrix size: %d x %d\n", lhs->rows, rhs->columns);
	worker_t workers[lhs->rows][rhs->columns];
	for (int i = 0; i < lhs->rows; i++) {
		for(int j = 0; j < rhs->columns; j++) {
			worker_t* current_worker = &workers[i][j];
			worker_data_t* worker_data = malloc(sizeof(worker_data_t));

			worker_data->reuslt_matrix_ptr = result_matrix;
			worker_data->cur_row_index = i;
			worker_data->cur_column_index = j;

			worker_data->fst_vector = lhs->data[i];
			worker_data->snd_vector = get_column(rhs, j);
			worker_data->vector_sizes = lhs->columns;
			current_worker->worker_data = worker_data;

			pthread_create(&current_worker->worker_thread, NULL, worker_routine, (void*)worker_data);
		}
	}

	for (int i = 0; i < lhs->rows; i++) {
		for (int j = 0; j < rhs->columns; j++) {
			worker_t* current_worker = &workers[i][j];
			pthread_join(current_worker->worker_thread, NULL);
			free((void*)current_worker->worker_data->snd_vector);
			free(current_worker->worker_data);
		}
	}
	return result_matrix;
}

int main(int argc, char** argv) {
	FILE* first_matrix_f = NULL, *second_matrix_f = NULL;
	matrix_t* first_matrix = NULL, *second_matrix = NULL, *result_matrix = NULL;
	if (argc != 3) {
		printf("Usage: %s <first_matrix_file> <second_matrix_file>\n", argv[0]);
		goto failure;
	}
	first_matrix_f = fopen(argv[1], "r");
	second_matrix_f = fopen(argv[2], "r");
	if (!first_matrix_f || !second_matrix_f) {
		printf("Could not open one of the files: %s, %s\n", argv[1], argv[2]);
		goto failure;
	}

	first_matrix = read_matrix_from_file(first_matrix_f);
	second_matrix = read_matrix_from_file(second_matrix_f);

	if (first_matrix->columns != second_matrix->rows) {
		printf("Could not multiply matrix. First matrix columns should be equal to second matrix rows\n");
		goto failure;
	}

	if (!first_matrix || !second_matrix) {
		printf("Failed to read one of matrix from file\n");
		goto failure;
	}

	result_matrix = multiply_matrix(first_matrix, second_matrix);
	print_matrix(result_matrix, stdout);

	fclose(first_matrix_f);
	fclose(second_matrix_f);
	delete_matrix(first_matrix);
	delete_matrix(second_matrix);
	delete_matrix(result_matrix);
	exit(EXIT_SUCCESS);
failure:
	if (first_matrix_f) {
		fclose(first_matrix_f);
	}
	if (second_matrix_f) {
		fclose(second_matrix_f);
	}

	delete_matrix(first_matrix);
	delete_matrix(second_matrix);
	delete_matrix(result_matrix);
	exit(EXIT_FAILURE);
}

