/* cdc75_fib.c (HW3)
 *
 *    Author: Chris Kasper
 *    Date: 01/29/18
 *
 *  This program utilizes pthreads to perform the Fibonacci sequence.
 *  Input: 1 parameter (int);
 *  Input determines how many numbers the program generates
 *  Output: Sequence of numbers generated
 *
 * Example runs:
 * ./fib 1
 * returns 0
 * ./fib 3 returns 0 1 1
 * ./fib 5 returns 0 1 1 2 3
 *
 * compile with: gcc -o fib cdc75_fib.c -lpthread
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct fib_data {
    int seq_num;
    int* fnum;
};

void* fib_entrypoint(void *arg)
{
    int i;

    // Cast f1 from void *arg to struct *fib
    struct fib_data *f1 = (struct fib_data*)arg;

    // Allocate memory for int* based on sequence numbers to generate
    if (f1->fnum != 0){
        f1->fnum = (int*) realloc(f1->fnum, f1->seq_num * sizeof(int));
    } else {
        f1->fnum = (int*) malloc(f1->seq_num  * sizeof(int));
    }

    // Perform Fibonacci calculation
    for (i = 0; i < f1->seq_num ; i++){
        if (i == 0) {
            f1->fnum[i] = 0;
        } else if (i == 1) {
            f1->fnum[i] = 1;
        } else {
            f1->fnum[i] = f1->fnum[i-1] + f1->fnum[i-2];
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t tid_fib;

    int num, ret, i;
    struct fib_data f1;

    // Get input from user
    if (argc == 2) {
      num = atoi(argv[1]);
      f1.seq_num = num;
    } else {
      printf("ERROR: this program accepts 1 parameter\n");
      exit(1);
    }

    // Create worker thread
    pthread_create(&tid_fib, NULL, fib_entrypoint, &f1);

    // Wait for worker thread to finish
    ret = pthread_join(tid_fib, NULL);
    if (ret) {
        fprintf(stderr, "error -- pthread_join() failed.\n");
        exit(EXIT_FAILURE);
    }

    // When worker thread finishes, output its results
    for(i = 0; i < f1.seq_num ; i++ ) {
        printf("%i ", f1.fnum[i]);
        if ((i + 1) == f1.seq_num) {
          printf("\n");
        }
    }

    // Free memory
    free(f1.fnum);

    return 0;
}
