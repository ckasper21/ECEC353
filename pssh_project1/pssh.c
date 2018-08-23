#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>

#include "builtin.h"
#include "parse.h"

/*******************************************
 * Set to 1 to view the command line parse *
 *******************************************/
#define DEBUG_PARSE 0
#define READ_SIDE 0
#define WRITE_SIDE 1

void print_banner ()
{
    printf ("                    ________   \n");
    printf ("_________________________  /_  \n");
    printf ("___  __ \\_  ___/_  ___/_  __ \\ \n");
    printf ("__  /_/ /(__  )_(__  )_  / / / \n");
    printf ("_  .___//____/ /____/ /_/ /_/  \n");
    printf ("/_/ Type 'exit' or ctrl+c to quit\n\n");
}


/* returns a string for building the prompt
 *
 * Note:
 *   If you modify this function to return a string on the heap,
 *   be sure to free() it later when appropirate!  */
static char* build_prompt ()
{
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL){
		fprintf(stdout, "%s", cwd);
	}
    	return  "$ ";
}


/* return true if command is found, either:
 *   - a valid fully qualified path was supplied to an existing file
 *   - the executable file was found in the system's PATH
 * false is returned otherwise */
static int command_found (const char* cmd)
{
    char* dir;
    char* tmp;
    char* PATH;
    char* state;
    char probe[PATH_MAX];

    int ret = 0;

    if (access (cmd, F_OK) == 0)
        return 1;

    PATH = strdup (getenv("PATH"));

    for (tmp=PATH; ; tmp=NULL) {
        dir = strtok_r (tmp, ":", &state);
        if (!dir)
            break;

        strncpy (probe, dir, PATH_MAX);
        strncat (probe, "/", PATH_MAX);
        strncat (probe, cmd, PATH_MAX);

        if (access (probe, F_OK) == 0) {
            ret = 1;
            break;
        }
    }

    free (PATH);
    return ret;
}

/* Called upon receiving a successful parse.
 * This function is responsible for cycling through the
 * tasks, and forking, executing, etc as necessary to get
 * the job done! */
void execute_tasks (Parse* P)
{
	unsigned int t;
	int prev_pipe;
	pid_t* pid;

	pid = malloc(P->ntasks * sizeof(*pid));

	for (t = 0; t < P->ntasks; t++) {
		int fd[2];
		if (pipe(fd) == -1) {
	        fprintf(stderr, "error -- failed to create pipe");
			exit(EXIT_FAILURE);
		}

		pid[t]  = vfork();

		if (pid < 0) {
			fprintf(stderr,"error -- failed to vfork()\n");
			exit(EXIT_FAILURE);
		} else if (pid[t] == 0) {
			if (t == 0 && P->ntasks > 1) { // If first task to be piped
				if (dup2(fd[WRITE_SIDE],STDOUT_FILENO) == -1) {
					fprintf(stderr, "error -- dup2() failed for WRITE_SIDE -> STDOUT\n");
					exit(EXIT_FAILURE);
				}
			} else if (t < (P->ntasks-1) && P->ntasks > 1) {
				if (dup2(prev_pipe,STDIN_FILENO) == -1) { //old pipe read to stdin
					fprintf(stderr, "error -- dup2() failed for READ_SIDE -> STDIN\n");
					exit(EXIT_FAILURE);
				}
				if (dup2(fd[WRITE_SIDE],STDOUT_FILENO) == -1) {
					fprintf(stderr, "error -- dup2() failed for WRITE_SIDE -> STDOUT\n");
					exit(EXIT_FAILURE);
				}
			} else if (t == (P->ntasks-1) && P->ntasks > 1) { // Last task to read from pipe
				if (dup2(prev_pipe,STDIN_FILENO) == -1) {
					fprintf(stderr, "error -- dup2() failed for READ_SIDE -> STDIN\n");
					exit(EXIT_FAILURE);
				}
			}
			if (is_builtin (P->tasks[t].cmd)) {
                if (P->outfile && t == (P->ntasks-1)){
                    FILE *fp_out;
					fp_out = fopen(P->outfile,"w");
					if (dup2(fileno(fp_out),STDOUT_FILENO) == -1) {
   	             		fprintf(stderr, "error -- dup2() failed for outfile -> STDOUT\n");
						exit(EXIT_FAILURE);
					}
			    }
                if (P->infile){
		            FILE *fp_in;
			        fp_in = fopen(P->infile,"r");
				   	if (dup2(fileno(fp_in),STDIN_FILENO) == -1) {
						fprintf(stderr, "error -- dup2() failed for infile -> STDIN\n");
	            	}
				}
				builtin_execute (P->tasks[t]);
			} else if (command_found (P->tasks[t].cmd)) {
				if (P->outfile && t == (P->ntasks-1)){
					FILE *fp_out;
					fp_out = fopen(P->outfile,"w");
					if (dup2(fileno(fp_out),STDOUT_FILENO) == -1) {
						fprintf(stderr, "error -- dup2() failed for outfile -> STDOUT\n");
						exit(EXIT_FAILURE);
					}
				}
				if (P->infile){
					FILE *fp_in;
					fp_in = fopen(P->infile,"r");
					if (dup2(fileno(fp_in),STDIN_FILENO) == -1) {
						fprintf(stderr, "error -- dup2() failed for infile -> STDIN\n");
					}
				}
				execvp(P->tasks[t].cmd,&P->tasks[t].argv[0]);
				printf ("pssh: found but can't exec: %s\n", P->tasks[t].cmd);
			} else {
				printf ("pssh: command not found: %s\n", P->tasks[t].cmd);
				exit(EXIT_SUCCESS);
			}
		} else {
			if (strcmp(P->tasks[t].cmd, "exit") == 0){
				builtin_execute (P->tasks[t]);
			}
			if (prev_pipe != 0){
				close(prev_pipe);
			}
			close(fd[WRITE_SIDE]);
			if (t == (P->ntasks-1)){
				close(fd[READ_SIDE]);
			}
			prev_pipe = fd[READ_SIDE];
		}
	}
	for (t = 0; t < P->ntasks; t++) {
		wait(NULL);
	}

	free(pid);
}

int main (int argc, char** argv)
{
    char* cmdline;
    Parse* P;

    print_banner ();

    while (1) {
        cmdline = readline (build_prompt());
        if (!cmdline)       /* EOF (ex: ctrl-d) */
            exit (EXIT_SUCCESS);

        P = parse_cmdline (cmdline);
        if (!P)
            goto next;

        if (P->invalid_syntax) {
            printf ("pssh: invalid syntax\n");
            goto next;
        }

#if DEBUG_PARSE
        parse_debug (P);
#endif

        execute_tasks (P);

    next:
        parse_destroy (&P);
        free(cmdline);
    }
}
