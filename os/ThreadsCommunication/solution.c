#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/uio.h>


/* Var 7 */
/*
7.     Взаємодія процесів. Паралелізм. Обмін повідомленнями. Обчислити f(x) * g(x), використовуючи 2 допоміжні процеси: один обчислює f(x), а інший – g(x). Основна програма виконує ввод-вивід та операцію *. Використати обмін повідомленнями між процесами (Messages). Реалізувати варіант блокуючих операцій обміну повідомленнями, тобто з очікуванням обробки повідомлення і відповіді на повідомлення (і “зависанням” процесу на цей час). Функції f(x) та g(x) “нічого не знають друг про друга” і не можуть комунікувати між собою.
*/

#define PROCESS_AMOUNT 2
#define WAIT_TIME_SEC 5

static int x_val;

int f(int);
int g(int);

typedef enum { TIMER_EXPIRED, FINISHED, WORKING, ERROR, INTERUPTED } WORKER_STATE;

typedef struct worker_proc {
	pid_t pid;
	int pipe_fd[2];
	int (*working_function)(int);

	WORKER_STATE state;

	pthread_t result_thread;
	pthread_t error_thread;
	pthread_mutex_t state_lock; /* Lock for WORKER_STATE shared resources */
	int result;

	int wait_timer;
} worker_proc_t;

bool is_all_workers_finished(const worker_proc_t*, int);
bool is_timer_expired(const worker_proc_t*, int, pid_t*);
void* result_handler(void*);
void* error_handler(void*);
worker_proc_t* get_worker_by_pid(worker_proc_t*, int, pid_t);
int reusult_function(const worker_proc_t*, int);

int main(void) {
	fprintf(stdout, "Enter x value to evaluate f(x) * g(x): ");
	fscanf(stdin, "%i", &x_val);
	fprintf(stdout, "\n");

	worker_proc_t workers[PROCESS_AMOUNT];
	/* Initialize states */
	for(int i = 0; i < PROCESS_AMOUNT; i++) {
		workers[i].state = WORKING;
		/* Customizable */
		workers[i].wait_timer = WAIT_TIME_SEC;
	}

	/* Initialize functors */
	workers[0].working_function = f;
	workers[1].working_function = g;
	for(int i = 2; i < PROCESS_AMOUNT; i++) { 
		workers[i].working_function = NULL;
	}

	/* Initialize pipes */
	for(int i = 0; i < PROCESS_AMOUNT; i++) {
		pipe(workers[i].pipe_fd);
	}

	/* Initialize child(workers) processes */
	for (int i = 0; i < PROCESS_AMOUNT; i++) {
		workers[i].pid = fork();
		if (workers[i].pid == 0) { /* Current process scope */
			close(workers[i].pipe_fd[0]); /* Close the reading-side of current process */
			for (int j = 0; j < PROCESS_AMOUNT; j++) { /* Close read/write-side for other processes */
				if (i != j) {
					close(workers[j].pipe_fd[0]);
					close(workers[j].pipe_fd[1]);
				}
			}

			fprintf(stdout, "Start evaluating function under %i process\n", (int)getpid());
			int result = workers[i].working_function(x_val);
			write(workers[i].pipe_fd[1], &result, sizeof(result));

			close(workers[i].pipe_fd[1]); /* Close writing side of current process */
			exit(EXIT_SUCCESS);
		}
	}

	/* Main process */
	for(int i = 0; i < PROCESS_AMOUNT; i++) { /* Close write-side of all workers processes */
		close(workers[i].pipe_fd[1]);
	}

	for(int i = 0; i < PROCESS_AMOUNT; i++) {
		pthread_create(&workers[i].result_thread, NULL, result_handler, (void*)&workers[i]);
		pthread_create(&workers[i].error_thread, NULL, error_handler, (void*)&workers[i]);
	}

	/* Busy wait loop */
	while(true) {
		pid_t expired_process;
		if (is_timer_expired(workers, PROCESS_AMOUNT, &expired_process)) {
			fprintf(stdout, "Timeout of process %i. Would you like to continue evaluating? [Y/N]\n", (int)expired_process);
			worker_proc_t* worker = get_worker_by_pid(workers, PROCESS_AMOUNT, expired_process);
			int ch;
			pthread_mutex_lock(&worker->state_lock);
			while((ch = getchar()) != EOF) {
				if ((char)ch == 'Y') {
					if (worker == NULL) {
						fprintf(stdout, "Worker with pid %i does not exist. Interupt...\n", expired_process);
						goto cleanup;
					}

					worker->state = WORKING;

					pthread_join(worker->error_thread, NULL); /* Resturt error handler (timer) thread */ 
					pthread_create(&worker->error_thread, NULL, error_handler, (void*)worker);
			
					pthread_mutex_unlock(&worker->state_lock);
					break;
				}
				if ((char)ch == 'N') {
					fprintf(stdout, "Evaluation interupted\n");
					worker->state = INTERUPTED;
					pthread_mutex_unlock(&worker->state_lock);
					goto cleanup;
				}
			}

			pthread_mutex_unlock(&worker->state_lock);
		}

		if (is_all_workers_finished(workers, PROCESS_AMOUNT)) {
			fprintf(stdout, "Result: %i\n", reusult_function(workers, PROCESS_AMOUNT));
			break;
		}
	}

cleanup:
	/* Cleanup */
	for(int i = 0; i < PROCESS_AMOUNT; i++) { /* Close reading-side of all workers processes */
		pthread_join(workers[i].result_thread, NULL);
		pthread_join(workers[i].error_thread, NULL);
	}

	exit(EXIT_SUCCESS);
}

worker_proc_t* get_worker_by_pid(worker_proc_t* workers, int size, pid_t pid) {
	for(int i = 0; i < size; i++) {
		if (workers[i].pid == pid) {
			return &workers[i];
		}
	}
	return NULL;
}

void* result_handler(void* worker_ptr) {
	worker_proc_t* worker = (worker_proc_t*)worker_ptr;
	if(read(worker->pipe_fd[0], &worker->result, sizeof(worker->result)) == -1) {
		//TODO: Error
	} else {
		if (worker->state == TIMER_EXPIRED) {
			fprintf(stdout, "Timeout expired. Stoped processing result of %i process...\n", (int)worker->pid); 
			while (worker->state == TIMER_EXPIRED) {
				/* Stop processing */
			}
		}
		if (worker->state != INTERUPTED) {
			pthread_mutex_lock(&worker->state_lock);
			fprintf(stdout, "Evaluating by process %i finished!\n", (int)worker->pid);
			worker->state = FINISHED;
			pthread_mutex_unlock(&worker->state_lock);
		}
	}
}

void* error_handler(void* worker_ptr) {
	worker_proc_t* worker = (worker_proc_t*)worker_ptr;
	while(worker->state != FINISHED || worker->state == INTERUPTED) {
		sleep(worker->wait_timer);
		/* Timer expired */
		pthread_mutex_lock(&worker->state_lock);
		if (worker->state != FINISHED) {
			worker->state = TIMER_EXPIRED;
		}
		pthread_mutex_unlock(&worker->state_lock);
		break;
	}
}

bool is_all_workers_finished(const worker_proc_t* workers, int size) {
	for(int i = 0; i < size; i++) {
		if (workers[i].state != FINISHED ) {
			return false;
		}
	}
	return true;
}

bool is_timer_expired(const worker_proc_t* workers, int size, pid_t* process_timer_expired) {
	for(int i = 0; i < size; i++) {
		if (workers[i].state == TIMER_EXPIRED) {
			(*process_timer_expired) = workers[i].pid;
			return true;
		}
	}
	return false;
}


int f(int x) {
	// Some work
	sleep(3);
	return x;
}

int g(int x) {
	// Some work
	sleep(3);
	return x;
}

int reusult_function(const worker_proc_t* workers, int size) {
	int res = 1;
	/* Varinat 7 just multiple */
	for (int i = 0; i < size; i++) {
		res *= workers[i].result;
	}
	return res;
}
