#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "lib/user/syscall.h"
#include "devices/shutdown.h"

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
    case SYS_HALT: halt();
                   break;

    case SYS_EXEC: 
                   break;

    case SYS_WRITE: break;
  }

  printf ("system call!\n");
  thread_exit ();
}


static void
_halt (void)
{
  shutdown_power_off ();
}
