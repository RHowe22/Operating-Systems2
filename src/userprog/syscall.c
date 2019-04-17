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
#include "file.h"
#include "lib/string.h"
#define STDOUT_FILENO 1
#define STDIN_FILENO 0
static void syscall_handler (struct intr_frame *);
 
void _exit (int status);
void _halt (void);
pid_t _exec (const char *file);
int _wait (pid_t);
bool _create (const char *file, unsigned initial_size);
bool _remove (const char *file);
int _open (const char *file);
int _filesize (int fd);
int _read (int fd, void *buffer, unsigned length);
int _write (int fd, const void *buffer, unsigned length);
void _seek (int fd, unsigned position);
unsigned _tell (int fd);
void _close (int fd);
struct lock filesys_lock;

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(&filesys_lock); //initialize the lock for file system
  activate_PIDlist();
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
  list_entry ( findPid(&allPID,(pid_t)t->tid) , struct parchild ,allpid)->retVal= status;
  printf("%s:exit(%d)",strtok_r(t->name," ",&saveptr),status);
  process_exit();
}

pid_t exec (const char *  cmd_line){
  return spawnChild(cmd_line,(pid_t)thread_current()->tid);
}


int wait (tid_t pid){
    struct parchild * cur = list_entry(findPid(&allPID,(pid_t)thread_current()->tid)
                  ,struct parchild, allpid) ;
    struct parchild * child;              
    // checks to see if the current process has a child with the pid_t pid
    for(struct list_elem * el = list_begin(&cur->childlist);el!=list_end(&cur->childlist);el= list_next(&cur->childlist)){
          child=list_entry(el,struct parchild,childelem);
          if(child->pidval== pid)
          {
            //if so return the return of that child
            return process_wait(pid);
          }

    }
    // if the child is not/or no longer in the parents child list return error
    return -1;
}
bool create (const char * file, unsigned initial_size){
  return filesys_create(file,initial_size);
}
bool remove (const char * file ){
  return filesys_remove(file);
}
int open (const char * file){
  return -1;
}
int filesize (int fd){
  struct file * file = findFD(fd);
  if(file != NULL){
    return file_length(file);
  }
  return -1;
}
int read (int fd, void *buffer, unsigned length){
  if (fd == STDIN_FILENO){
    char c;
      for (unsigned i =0; i < length; i++)
      {
          c= input_getc();
           if(c == '\n')
              return ((int) i);
          else
          {
            *((char*)buffer+i)=c;
          }    
      }
      return (int) length;
  }
  else{
    struct file * myFile = findFD(fd);
    returnfile_read(myFile, buffer, length);
  }
}


int write (int fd, const void * buffer, unsigned size){
  if(fd == STDOUT_FILENO){
    putbuf(buffer,size);
    return size;
  }
  else{
    struct file * myFile = findFD(fd);
    return file_write (myFile, buffer, size);
  }
}

void seek (int fd, unsigned position){


}
unsigned tell (int fd){
  return 0;
}
void close (int fd) {
  lock_acquire(&filesys_lock);
  

}
