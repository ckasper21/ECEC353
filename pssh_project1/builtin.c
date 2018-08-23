#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"
#include "parse.h"

static char* builtin[] = {
    "exit",   /* exits the shell */
    "which",  /* displays full path to command */
    NULL
};


int is_builtin (char* cmd)
{
    int i;

    for (i=0; builtin[i]; i++) {
        if (!strcmp (cmd, builtin[i]))
            return 1;
    }

    return 0;
}


void builtin_execute (Task T)
{
    if (!strcmp (T.cmd, "exit")) {
		exit (EXIT_SUCCESS);

    } else if (!strcmp(T.cmd, "which")){
		if (T.argv[1]==NULL){
			exit(EXIT_SUCCESS);
		}

		if (is_builtin(T.argv[1])){
		   printf("%s: shell built-in command\n", T.argv[1]);
           exit(EXIT_SUCCESS);
		}
		char* paths = (char *) malloc(sizeof(char*)*strlen(getenv("PATH")));
		strcpy(paths,getenv("PATH"));
		char* p;
		
		p = strtok(paths,":");
		while (p != NULL) {
			char* cp_tok = (char *) malloc(sizeof(char*)*strlen(p));
			strcpy(cp_tok,p);
			strcat(cp_tok,"/");
			strcat(cp_tok,T.argv[1]);

			if (access(cp_tok, F_OK)==0){
				printf("%s\n",cp_tok);
				free(paths);
				free(cp_tok);
				exit(EXIT_SUCCESS);
			}
			free(cp_tok);
		   	p = strtok(NULL,":");
		}
		free(paths);
		exit(EXIT_SUCCESS);
	
	} else {
        printf ("pssh: builtin command: %s (not implemented!)\n", T.cmd);
    }
}
