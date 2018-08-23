/* fork.c (HW1)
 * 
 *   Author: Chris Kasper
 *     Date: 01/15/18
 *     Note: commented out the print statements for measurement purposes
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	int forks = 1;

    while (forks <= 10) { // Performs 10 forks
    	pid_t pid;
	
//	printf("fork #%i\n",forks);

    	pid = fork();
    	if (pid < 0) {
        	fprintf(stderr, "error -- failed to fork()");
        	return 1;
    	}	

    	if (pid > 0) {
        	/* only executed by the PARENT process */
        	int child_ret;

        	waitpid(pid, &child_ret, 0);
//		printf("parent process executing...\n");
//        	printf("parent pid -- %i\n", pid);
//        	printf("child process exited with code %i\n", child_ret);

    	} else {
        	/* only executed by the CHILD process */
//		printf("child process executing...\n");
        	exit (0);
    	}
	forks++;
//	printf("\n");
    }
    return 0;
}
