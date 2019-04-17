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
    struct FDPair   // key value pairing of file descriptor to actual file
    {
      int fd;
      struct file * fileval;
    } openfilelists [128];
    uint8_t numFD;    // number of File Descriptors the file has 
    int  nextFD;
};
tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);
void activate_PIDlist(void);
pid_t spawnChild (const char * cmdline, pid_t parPID );

struct parchild * findPid(pid_t);
struct parchild *  findchild (struct list *, pid_t);
struct file * findFD (int);

#endif /* userprog/process.h */
