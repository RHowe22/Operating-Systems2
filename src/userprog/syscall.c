#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "lib/user/syscall.h"
#include "lib/kernel/stdio.h"
#include "devices/shutdown.h"
#include "filesys/filesys.h"
#include "process.h"
#include "lib/string.h"
#define STDOUT_FILENO 0
#define STDIN_FILENO 1
static void syscall_handler (struct intr_frame *);
 
void _exit (int status);
static void _halt (void);
static pid_t _exec (const char *file);
static int _wait (pid_t);
static bool _create (const char *file, unsigned initial_size);
static bool _remove (const char *file);
static int _open (const char *file);
static int _filesize (int fd);
static int _read (int fd, void *buffer, unsigned length);
static int _write (int fd, const void *buffer, unsigned length);
static void _seek (int fd, unsigned position);
static unsigned _tell (int fd);
static void _close (int fd);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}


static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  int * esp = (int)f->esp;

  int syscall_num = *esp;
  switch (syscall_num) {
    //handler for exit
    case SYS_EXIT: exit (*(esp + 1));
                   break;

    //handler for halt
    case SYS_HALT: halt();
                   break;
               
    //handler for exec
    case SYS_EXEC: f->eax = exec ((char *) *(esp + 1));
                   break;

    //handler for wait
    case SYS_WAIT: f->eax = wait (*(esp + 1));
                   break;

    //handler for create
    case SYS_CREATE: f->eax = create ((char *) *(esp + 1), *(esp + 2));
                     break;

    //handler for remove
    case SYS_REMOVE: f->eax = remove ((char *) *(esp + 1));
                     break;

    //handler for open
    case SYS_OPEN: f->eax = open ((char *) *(esp + 1));
                   break;

    //handler for filesize
    case SYS_FILESIZE: f->eax = filesize (*(esp + 1));
                       break;

    //handler for read
    case SYS_READ: f->eax = read (*(esp + 1), (void *) *(esp + 2), *(esp + 3));
                   break;

    //handler for write
    case SYS_WRITE: f->eax = write(*(esp+1), (void *) *(esp+2), *(esp+3));
                    break;

    //handler for sys_seek
    case SYS_SEEK: seek (*(esp + 1), *(esp + 2));
                   break; 

    //handler for sys_tell
    case SYS_TELL: f->eax = tell (*(esp + 1));
                   break;

    //handler for sys_close
    case SYS_CLOSE: close (*(esp + 1));
                    break;
  }

  printf ("system call!\n");
  thread_exit ();
}

void halt (void){
  shutdown();
}


void exit (int status){
  struct thread * t = thread_current();
  char * saveptr;
  printf("%s:exit(%d)",strtok_r(t->name," ",saveptr),status);
  thread_exit();
}

pid_t exec (const char *  cmd_line){
  return 0;
}


int wait (tid_t pid){
  if(true) // should do checking here to see if the child
    return process_wait(pid);
  else
  {
    exit(-1);
  }
    
}
bool create (const char * file, unsigned initial_size){
  return filesys_create(file,initial_size);
}
bool remove (const char * file ){
  return false;
}
int open (const char * file){
  return -1;
}
int filesize (int fd){
  return -1;
}
int read (int fd, void *buffer, unsigned length){
  if (fd == STDIN_FILENO){
      input_getc();
  }
  else
    return -1;
}


int write (int fd, const void * buffer, unsigned size){
  if(fd == STDOUT_FILENO){
    putbuf(buffer,size);
    return size;
  }
  else
    return -1;
}

void seek (int fd, unsigned position){

}
unsigned tell (int fd){
  return 0;
}
void close (int fd){

}
