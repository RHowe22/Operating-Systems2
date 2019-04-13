#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"
#include "lib/kernel/list.h"
#include "threads/synch.h"

typedef int pid_t;
#define PID_ERROR ((pid_t) -1)

struct parchild{
    pid_t pidval;
    int retVal;
    pid_t parPid;
    struct list_elem  allpid;
    struct list_elem childelem;
    struct list childlist;
    struct semaphore parentwaiting;
};
tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);
void activate_PIDlist(void);
pid_t spawnChild (char * cmdline, pid_t parPID );

struct list_elem * findPid(pid_t);
#endif /* userprog/process.h */
