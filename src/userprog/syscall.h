#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "lib/user/syscall.h"

void syscall_init(void);
void check_address(const void *vaddr);

struct lock filesys_lock;

#endif /* userprog/syscall.h */
