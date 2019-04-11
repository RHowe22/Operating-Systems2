#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "lib/user/syscall.h"
#include "lib/kernel/stdio.h"
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

}
void exit (int status){

}
pid_t exec (const char *  cmd_line){
  return 0;
}
int wait (tid_t pid);
bool create (const char * file, unsigned initial_size){
  return false;
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
int write (int fd, const void * buffer, unsigned size){
  if(fd == 1){
    putbuf(buffer,size);
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