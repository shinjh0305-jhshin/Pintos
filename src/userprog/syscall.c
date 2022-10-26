#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

/*Pintos 1_User program_include --------------------------------- STARTS HERE*/
#include "userprog/process.h"
#include <list.h>
/*Pintos 1_User program_include --------------------------------- ENDS HERE*/


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

/*Pintos 1_User program_exec, wait, exit --------------------------------- STARTS HERE*/
tid_t exec(const char* task) {
  tid_t child_tid = process_execute(task);
  struct thread* child = get_child_process(child_tid);

  sema_down(&(child->load)); //wait
  if (child->isLoaded == true) {
    return child_tid;
  } else {
    return -1;
  }
}

void exit(int status) {
  struct thread* thread = thread_current();
  thread->exitStatus = status;

  printf("%s: exit(%d)\n", thread->name, status);
  thread_exit();
}

int wait(tid_t tid) {
  return process_wait(tid);
}
/*Pintos 1_User program_exec, wait, exit --------------------------------- ENDS HERE*/

