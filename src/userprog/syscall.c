#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"

/*Pintos 1_User program_include --------------------------------- STARTS HERE*/
#include "userprog/process.h"
#include "devices/shutdown.h"
#include "devices/input.h"
#include "userprog/pagedir.h"
#include "threads/vaddr.h"
#include <list.h>
/*Pintos 1_User program_include --------------------------------- ENDS HERE*/


static void syscall_handler (struct intr_frame *);

/*Pintos 1_User program_Check address, Get Argument --------------------------------- STARTS HERE*/
//esp가 유효한 주소인지 확인한다.
void check_address(const void* addr) {
  uint32_t* pd = thread_current()->pagedir;

  //check unmapped memory
  void* mapped_paddr = pagedir_get_page(pd, addr);
  if (mapped_paddr == NULL) { //unmapoped or fishy mapping
    exit(-1);
  }

  //check kernel addr
  if (addr == NULL || !is_user_vaddr(addr) || (unsigned int *)addr < (unsigned int *) 0x8048000){
		exit(-1);
	}
}

//argument를 저장한다.
void getArgument(void* esp, int* arg, int count) {
  void* mov = esp;
  mov += 4;

  for (int i = 0; i < count; i++) {
    check_address(mov);
    arg[i] = *(int*) mov;
    mov += 4;
  }
}
/*Pintos 1_User program_Check address, Get Argument --------------------------------- ENDS HERE*/

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{ 
  //printf("Entering syscall_handler.\n");
  int argv[4];
  check_address(f->esp);
  uint32_t syscallNum = *(int*)(f->esp);

  switch(syscallNum) {
    case SYS_HALT :
      halt();
      break;
    case SYS_EXIT :
      getArgument(f->esp, argv, 1);
      exit(argv[0]);
      break;
    case SYS_EXEC :
      getArgument(f->esp, argv, 1);
      check_address((void*)argv[0]);
      f->eax = exec((const char*)argv[0]);
      break;
    case SYS_WAIT : 
      getArgument(f->esp, argv, 1);
      f->eax = wait(argv[0]);
      break;
    case SYS_READ :
      getArgument(f->esp, argv, 3);
      f->eax = read(argv[0], (void*) argv[1], (unsigned)argv[2]);
      break;
    case SYS_WRITE :
      getArgument(f->esp, argv, 3);
      f->eax = write(argv[0], (void*) argv[1], (unsigned)argv[2]);
      break;
    case USR_FIBONACCI : 
      getArgument(f->esp, argv, 1);
      f->eax = fibonacci(argv[0]);
      break; 
    case USR_MAXOF4 : 
      getArgument(f->esp, argv, 4);
      f->eax = max_of_four_int(argv[0], argv[1], argv[2], argv[3]);
      break;
    default :
      break;
  }
}

/*Pintos 1_User program_exec, wait, exit, halt, read, write--------------------------------- STARTS HERE*/
tid_t exec(const char* task) {
  tid_t child_tid = process_execute(task);
  
  if (child_tid == TID_ERROR) return TID_ERROR;
  
  struct thread* child = get_child_process(child_tid);
  
  sema_down(&(child->load)); //wait
  if (child->isLoaded == true) {
    return child_tid;
  } else {
    return TID_ERROR;
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

void halt() {
  shutdown_power_off();
}

int read(int fd, void* buffer, unsigned size) {
  int len = 0;

  ASSERT(fd == 0);

  char ch;
  for (unsigned i = 0; i < size; i++) {
    ch = ((char*)buffer)[i] = input_getc();
    if (ch == '\n' || ch == '\0') {
      ((char*)buffer)[i] = '\0';
      break;
    }
    else {
      len++;
    }
  }

  return len;
}

int write(int fd, void* buffer, unsigned size) {
  ASSERT(fd == 1);
  putbuf(buffer, size);
  return size;
}
/*Pintos 1_User program_exec, wait, exit, halt, read, write--------------------------------- ENDS HERE*/


/*Pintos 1_User program_fibonacci, max --------------------------------- STARTS HERE*/
int fibonacci(int num) {
  int fib;

  if (num <=2) fib = 1;
  else {
    int fib_1 = 1, fib_2 = 1;
    int temp;
    
    for (int i = 3; i <= num; i++) {
        temp = fib_1 + fib_2;
        fib_1 = fib_2;
        fib_2 = temp;
    }

    fib = fib_2;
  }

  return fib;
}

int max_of_four_int(int num1, int num2, int num3, int num4) {
  int max = num1;

  max = max > num2 ? max : num2;
  max = max > num3 ? max : num3;
  max = max > num4 ? max : num4;

  return max;
}
/*Pintos 1_User program_fibonacci, max  --------------------------------- ENDS HERE*/
