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

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
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
}
int write (int fd, const void * buffer, unsigned size){
  if(fd == STDOUT_FILENO){
    putbuf(buffer,size);
    return size;
  }
  else
    return -1;
}
void seek (int fd, unsigned postion){

}
unsigned tell (int fd){
  return 0;
}
void close (int fd){

}