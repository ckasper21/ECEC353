#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define BUFFER_SIZE 25
#define READ_SIDE 0
#define WRITE_SIDE 1

int main(void)
{
    char to_child[BUFFER_SIZE];
    char to_parent[BUFFER_SIZE];
    char EXIT[5]="exit";
    int fd1[2],fd2[2]; // fd1 is pipe1 parent(w) to child(r)
                      // fd2 is pipe2 child(w) to parent(r)
    pid_t pid;

    while (1) {
    /* create the pipe */
        if (pipe(fd1) == -1) {
            fprintf(stderr, "error -- failed to create pipe1");
            return 1;
        }
        if (pipe(fd2) == -1) {
            fprintf(stderr, "error -- failed to create pipe2");
            return 1;
        }

        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "error -- failed to fork()");
            return 1;
        }

        if (pid > 0) { /* parent process */
            printf("Enter some text: ");
            scanf("%[^\n]%*c",to_child);

            if (strcmp(to_child,EXIT) == 0) {
                exit(0);
            }

            int child_ret;
            int ret;

            close(fd1[READ_SIDE]);
            close(fd2[WRITE_SIDE]);

            write(fd1[WRITE_SIDE], to_child, strlen(to_child)+1);
            wait(&child_ret);

            ret = read(fd2[READ_SIDE], to_parent, BUFFER_SIZE);
            printf("%s\n", to_parent);

            close(fd1[WRITE_SIDE]);
            close(fd2[READ_SIDE]);

        } else {       /* child process */
            int ret;
            int i=0;

            close(fd1[WRITE_SIDE]);
            close(fd2[READ_SIDE]);

            ret = read(fd1[READ_SIDE], to_parent, BUFFER_SIZE);

            while(to_parent[i]){ // Converts string to uppercase
              to_parent[i]= toupper(to_parent[i]);
              i++;
            }

            write(fd2[WRITE_SIDE], to_parent, strlen(to_parent)+1);
            close(fd1[READ_SIDE]);
            close(fd2[WRITE_SIDE]);

            exit (0);
        }
    }
}
