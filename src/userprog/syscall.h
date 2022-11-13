#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "lib/user/syscall.h"
void syscall_init(void);

struct lock filesys_lock;

void check_address(const void *addr);
void getArgument(void *esp, int *arg, int count);
#endif /* userprog/syscall.h */
