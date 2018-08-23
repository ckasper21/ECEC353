/* signal.c (HW4)
 *
 *   Author: Chris Kasper
 *     Date: 02/05/18
 */

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main (int argc, char** argv)
{
	pid_t pid;
	signed int sig;
	int e_check,i;

	if (argc < 2) {
    	printf("Usage: ./signal [options] <pid>\n");
		printf("\nOptions:\n-s <signal>	Sends <signal> to <pid>\n");
		printf("-l		Lists all signal number with their names\n");
     	exit(1);
    }		
	
	if (!strcmp(argv[1],"-s")){
		sig = atoi(argv[2]);
		pid = atoi(argv[3]);
		if (sig == 0) {
			kill(pid,0);
			e_check = 1;			
		} else {
			kill(pid,sig);
		}
	
	} else if (!strcmp(argv[1],"-l")) {
		for (i = 1; i < (sizeof(sys_siglist)/sizeof(sys_siglist[0])); i++) {
			printf("%i: %s\n",i, sys_siglist[i]);
		}

	} else {
		sig = SIGTERM;
		pid = atoi(argv[1]);
		kill(pid,sig);
	}
	
	if (e_check == 1){
		if (errno == EPERM) {
			printf("PID %i exists, but we can't send it signals\n",pid);
		} else if (errno == ESRCH) {
			printf("PID %i does not exist\n",pid);
		} else {
			printf("PID %i exists and is able to receive signals\n",pid);
		}
	}
	return 0;	
}
