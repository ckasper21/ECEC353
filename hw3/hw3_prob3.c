#include <pthread.h>
#include <stdio.h>
#include <string.h>

struct thread_data {
    char name[255];
    unsigned int num_iterations;
    unsigned int some_answer;
};

void* thread_entrypoint(void *arg)
{
    int i;
    struct thread_data *tdata = (struct thread_data*)arg;

    tdata->some_answer = 0;
    for (i=0; i<tdata->num_iterations; i++) {
        tdata->some_answer += i;
    }

    printf("Name: %s, Answer: %u\n", tdata->name, tdata->some_answer);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t tid;
    struct thread_data tdata;

    strcpy(tdata.name, "Spartacus");
    tdata.num_iterations = 100;

    pthread_create(&tid, NULL, thread_entrypoint, &tdata);
    pthread_detach(pthread_self());
    pthread_exit(NULL);
}
