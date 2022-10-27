#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);

/*Pintos 1_User program_User defined function --------------------------------- STARTS HERE*/
#include "threads/thread.h"
void check_address(const void* addr);
void getArgument(void* esp, int* arg, int count);

tid_t exec(const char* task);
void exit(int status);
int wait(tid_t tid);
void halt();
int read(int fd, void* buffer, unsigned size);
int write(int fd, void* buffer, unsigned size);
int fibonacci(int num);
int max_of_four_int(int num1, int num2, int num3, int num4);
/*Pintos 1_User program_User defined function --------------------------------- ENDS HERE*/

#endif /* userprog/syscall.h */
