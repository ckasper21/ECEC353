#ifndef _job_h_
#define _job_h_

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

typedef enum {
	TERM,
	STOPPED,
	BG,
	FG,
} JobStatus;

typedef struct {
	char* name;
	pid_t* pids;
	unsigned int npids;
	unsigned int pids_left;
	pid_t pgid;
	JobStatus status;
} Job;

#endif
