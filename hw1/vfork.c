/* vfork.c (HW1)
 * 
 *   Author: Chris Kasper
 *     Date: 01/15/18
 *     Note: commented out print statements for measurement purposes
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	int vforks = 1;

	while (vforks<=10){
		pid_t pid;
//		printf("vfork #%i\n",vforks);
    		pid = vfork();

    		if (pid < 0) {
        		fprintf(stderr, "error -- failed to vfork()");
        		return 1;
    		}

    		if (pid > 0) {
        		/* only executed by the PARENT process */
        		int child_ret;
//			printf("parent process executing...\n");
//        		printf("parent -- pid: %i\n", getpid());
        		waitpid(pid, &child_ret, 0);
//        		printf("parent -- child exited with code %i\n", child_ret);
    		} else {
        		/* only executed by the CHILD process */
//			printf("child process executing...\n");
//			printf("child -- pid: %i\n", getpid());
			exit(0);
    		}
		vforks++;
//		printf("\n");
	}
	return 0;
}
