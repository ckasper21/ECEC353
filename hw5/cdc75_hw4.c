/*
 * HW5 - ECEC353
 * Author: Chris Kasper
 *
 * compile with gcc -o hw5 cdc75_hw5.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#define NUM_WORKER_THREADS 500

int result[NUM_WORKER_THREADS];

struct thread_data {
	pthread_t tid;
	unsigned int num;
};

struct workers_state {
	int still_working;
	pthread_mutex_t mutex;
	pthread_cond_t signal;
};

static struct workers_state wstate = {
	.still_working = NUM_WORKER_THREADS,
	.mutex = PTHREAD_MUTEX_INITIALIZER,
	.signal = PTHREAD_COND_INITIALIZER
};

void* worker_thread (void* param)
{
	struct thread_data* thread = (struct thread_data*) param;
	pthread_mutex_lock(&wstate.mutex);
	
	// Critical Section
	
	result[thread->num] = thread->num * thread->num;
	wstate.still_working--; 
	
	pthread_mutex_unlock(&wstate.mutex);
	pthread_cond_broadcast(&wstate.signal);

	pthread_exit(EXIT_SUCCESS);
}

int main (int argc, char** argv)
{
	struct thread_data* threads = malloc(sizeof(struct thread_data)*NUM_WORKER_THREADS);
	int i,prevnum;
	int total = 0;
	
	// Iterate through struct
	for (i = 0; i < NUM_WORKER_THREADS; i++){
		prevnum = wstate.still_working;
		threads[i].num = i;
		pthread_create(&threads[i].tid, NULL, worker_thread, &threads[i]);
		pthread_detach(threads[i].tid);

		while (wstate.still_working == prevnum){
			pthread_cond_wait(&wstate.signal, &wstate.mutex);
		}
	}
	
	free(threads);

	// Find the total of result array	
	for (i = 0; i < NUM_WORKER_THREADS; i++){
		total = total + result[i];
	}
	printf("Total = %i\n",total);

	// Find expected total
	int expect_total = 0 , sq_i = 0;

	for (i = 0; i < NUM_WORKER_THREADS; i++){
		sq_i = i * i;
		expect_total = expect_total + sq_i;
	}	
	printf("Expected total = %i\n", expect_total);
}
