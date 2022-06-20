#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#ifdef __STDC_NO_ATOMICS__
# error this implementation needs atomics
#endif
#include <stdatomic.h>
#include <time.h>

#define INCREMENT_SIZE 100000
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void atomic_counter(_Atomic int* val) {
	for(int i = 0; i < INCREMENT_SIZE; i++) {
		++val;
	}
}

void* atomic_worker(void* thread_data) {
	_Atomic int* val = (_Atomic int*)thread_data;
	atomic_counter(val);
	return NULL;
}

void mutex_counter(int* val) {
	for(int i = 0; i < INCREMENT_SIZE; i++) {
		pthread_mutex_lock(&mutex);
		(*val) = (*val) + 1;
		pthread_mutex_unlock(&mutex);
	}
}

void* mutex_worker(void* thread_data) {
	int* val = (int*)thread_data;
	mutex_counter(val);
	return NULL;
}

#define measure(func) \
	time_t start = time(NULL); \
	func; \
	time_t end = time(NULL); \
	printf("Took %f seconds\n", difftime(end, start));

int main(int argc, char** argv) {

	time_t start_atomic = time(NULL);
	_Atomic int* value_a = 0;
	pthread_t first_atomic_th, second_atomic_th;
	pthread_create(&first_atomic_th, NULL, atomic_worker,(void*)value_a);
	pthread_create(&second_atomic_th, NULL, atomic_worker,(void*)value_a);
	time_t end_atomic = time(NULL);
	printf("Took %f seconds on atomic evaluation\n", difftime(end_atomic, start_atomic));

	time_t start_mutex = time(NULL);
	int value_m = 0;
	pthread_t first_mutext_th, second_mutex_th;
	pthread_create(&first_mutext_th, NULL, mutex_worker, (void*)&value_m);
	pthread_create(&second_mutex_th, NULL, mutex_worker, (void*)&value_m);
	time_t end_mutex = time(NULL);
	printf("Took %f seconds on mutex evaluation\n", difftime(end_mutex, start_mutex));
	return 0;
}
