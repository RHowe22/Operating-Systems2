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
#include "filesys/file.h"
#include "lib/string.h"
#include "lib/stdio.h"
#include "devices/input.h"
#include "threads/palloc.h"

static void syscall_handler (struct intr_frame *);
 
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
  int * esp = (int *)f->esp;
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
    case SYS_WAIT: f->eax = wait ((tid_t) *(esp + 1));
                   break;

    //handler for create
    case SYS_CREATE: f->eax = create ((char *) *(esp + 1),(unsigned) *(esp + 2));
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
    case SYS_SEEK: seek (*(esp + 1),(unsigned) *(esp + 2));
                   break; 

    //handler for sys_tell
    case SYS_TELL: f->eax = tell (*(esp + 1));
                   break;

    //handler for sys_close
    case SYS_CLOSE: close (*(esp + 1));
                    break;
  }
}

void halt (void){
  shutdown_power_off();
}


void exit (int status){
  struct thread * t = thread_current();
  char * saveptr;
   findPid((pid_t)t->tid)->retVal= status;
  printf("%s: exit(%d)\n",strtok_r(t->name," ",&saveptr),status);
  thread_exit();
}

pid_t exec (const char *  cmd_line){
  return spawnChild(cmd_line,(pid_t)thread_current()->tid);
}


int wait (tid_t pid){
  int retval =-1;
    struct parchild * cur = findPid((pid_t)thread_current()->tid);
    struct parchild * child =findchild(&cur->childlist,pid);
    if(cur != NULL && child != NULL){
       retval= process_wait(pid);
       list_remove(&child->childelem);
       list_remove(&child->allpid);
       while(!list_empty(&child->childlist)){
         list_pop_front(&child->childlist);
       }
       palloc_free_page(child);
       child=NULL;
    }


    // if the child is not/or no longer in the parents child list return error 
    //other wise return the child's retval
    return retval;
}
bool create (const char * file, unsigned initial_size){
  return filesys_create(file,initial_size);
}
bool remove (const char * file ){
  return filesys_remove(file);
}
int open (const char * file){
  struct parchild * cur = findPid((pid_t)thread_current()->tid);
  if(cur!=NULL && cur->numFD <128)
  {
    struct file * toadd = filesys_open(file);
    if(toadd != NULL){
      cur->openfilelists[cur->numFD].fileval=toadd;
      cur->openfilelists[cur->numFD].fd=(cur->nextFD+1);
      cur-> numFD =  cur-> numFD +1;
      return (cur->nextFD= cur->nextFD+1);
    }
  }
  return -1;  // result of file has 128 files open
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
    if(myFile!= NULL)
      return file_read(myFile, buffer, length);
    return -1;  
  }
}


int write (int fd, const void * buffer, unsigned size){
  if(fd == STDOUT_FILENO){
    putbuf(buffer,size);
    return size;
  }
  else{
    struct file * myFile = findFD(fd);
    if(myFile!= NULL)
      return file_write (myFile, buffer, size);
  return -1;  
  }
}

void seek (int fd, unsigned postion){
  struct file * file = findFD(fd);
  if(file!= NULL){
    file_seek(file,(off_t)postion);
  }
}
unsigned tell (int fd){
  struct file * file = findFD(fd);
  if(file!= NULL){
    return (unsigned )file_tell(file);
  }
  return 0;
}
void close (int fd){
  //After closing File, also close FD
  struct parchild * cur = findPid((pid_t) thread_current()->tid);
  int index;
  for(index=0; index < cur->numFD; index++){
      if(cur->openfilelists[index].fd==fd){
        //Close file.
        file_close(cur->openfilelists[index].fileval);
        index = index + 1;
        for(; index < cur->numFD; index++){
          cur->openfilelists[index-1] = cur->openfilelists[index];
        }
        cur->numFD = cur->numFD -1;
        break;
      }
  }
  
}
