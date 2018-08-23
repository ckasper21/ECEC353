#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define READ_SIDE 0
#define WRITE_SIDE 1

int main(void)
{
    int fd[2];

    /* create the pipe */
    if (pipe(fd) == -1) {
        fprintf(stderr, "error -- failed to create pipe");
        return 1;
    }

    printf("Parent says, \"Doing 'ls -l | wc -l'\"\n");

    /* create child process for 'ls' */
    switch (fork()) {
    case -1:
        fprintf(stderr, "error -- failed to fork()");
        exit(EXIT_FAILURE);

    case 0:
        if (dup2(fd[WRITE_SIDE], STDOUT_FILENO) == -1) {
            fprintf(stderr, "error -- dup2() failed for WRITE_SIDE -> STDOUT");
            exit(EXIT_FAILURE);
        }
        close(fd[WRITE_SIDE]);

        execlp("ls", "ls", "-l", NULL);
        fprintf(stderr, "error -- exec(ls) failed!");

    default:
        break;
    }

    switch (fork()) {
    case -1:
        fprintf(stderr, "error -- failed to fork()");
        exit(EXIT_FAILURE);

    case 0:
        close(fd[WRITE_SIDE]);

        if (dup2(fd[READ_SIDE], STDIN_FILENO) == -1) {
            fprintf(stderr, "error -- dup2() failed for READ_SIDE -> STDIN");
            exit(EXIT_FAILURE);
        }
        close(fd[READ_SIDE]);

        execlp("wc", "wc", "-l", NULL);
        fprintf(stderr, "error -- exec(wc) failed!");2

    default:
        break;
    }

    close(fd[READ_SIDE]);
    close(fd[WRITE_SIDE]);

    wait(NULL);
    wait(NULL);

    return 0;
}
