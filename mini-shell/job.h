#ifndef JOB_H
#define JOB_H

#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define PROCESS_INPROGRESS 1
#define PROCESS_STOPPED 0

struct JOB
{
    pid_t job_id;
    pid_t pid;
    int status;
    char process_name[100];
    struct JOB* next;
};

typedef struct JOB JOB;

#endif
