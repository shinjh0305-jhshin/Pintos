#include "userprog/syscall.h"

#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>

/*Pintos 1_User program_include --------------------------------- STARTS HERE*/
#include <list.h>

#include "devices/input.h"
#include "devices/shutdown.h"
#include "threads/interrupt.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
/*Pintos 1_User program_include --------------------------------- ENDS HERE*/

static void syscall_handler(struct intr_frame *);

void syscall_init(void) {
    lock_init(&filesys_lock);
    intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler(struct intr_frame *f UNUSED) {
    switch (*(uint32_t *)(f->esp)) {
        case SYS_HALT:
            halt();
            break;

        case SYS_EXIT:
            check_address(f->esp + 4);
            exit(*(uint32_t *)(f->esp + 4));
            break;

        case SYS_EXEC:
            check_address(f->esp + 4);
            f->eax = exec((const char *)*(uint32_t *)(f->esp + 4));
            break;

        case SYS_WAIT:
            check_address(f->esp + 4);
            f->eax = wait((pid_t *)*(uint32_t *)(f->esp + 4));
            break;

        case SYS_CREATE:
            check_address(f->esp + 4);
            check_address(f->esp + 8);
            f->eax = create((const char *)*(uint32_t *)(f->esp + 4), (unsigned)*(uint32_t *)(f->esp + 8));
            break;

        case SYS_REMOVE:
            check_address(f->esp + 4);
            f->eax = remove((const char *)*(uint32_t *)(f->esp + 4));
            break;

        case SYS_OPEN:
            check_address(f->esp + 4);
            f->eax = open((const char *)*(uint32_t *)(f->esp + 4));
            break;

        case SYS_FILESIZE:
            check_address(f->esp + 4);
            f->eax = filesize((int)*(uint32_t *)(f->esp + 4));
            break;

        case SYS_READ:
            check_address(f->esp + 4);
            check_address(f->esp + 8);
            check_address(f->esp + 12);
            f->eax = read((int)*(uint32_t *)(f->esp + 4), (void *)*(uint32_t *)(f->esp + 8), (unsigned)*(uint32_t *)(f->esp + 12));
            break;

        case SYS_WRITE:
            check_address(f->esp + 4);
            check_address(f->esp + 8);
            check_address(f->esp + 12);
            f->eax = write((int)*(uint32_t *)(f->esp + 4), (void *)*(uint32_t *)(f->esp + 8), (unsigned)*((uint32_t *)(f->esp + 12)));
            break;

        case SYS_SEEK:
            check_address(f->esp + 4);
            check_address(f->esp + 8);
            seek((int)*(uint32_t *)(f->esp + 4), (unsigned)*(uint32_t *)(f->esp + 8));
            break;

        case SYS_TELL:
            check_address(f->esp + 4);
            f->eax = tell((int)*(uint32_t *)(f->esp + 4));
            break;

        case SYS_CLOSE:
            check_address(f->esp + 4);
            close((int)*(uint32_t *)(f->esp + 4));
            break;

        case SYS_SIGACTION:
            check_address(f->esp + 4);
            check_address(f->esp + 8);

            sigaction((int)*(uint32_t *)(f->esp + 4), (void *)*(uint32_t *)(f->esp + 8));

            break;

        case SYS_SENDSIG:
            check_address(f->esp + 4);
            check_address(f->esp + 8);

            sendsig((pid_t) * (uint32_t *)(f->esp + 4), (int)*(uint32_t *)(f->esp + 8));

            break;

        case SYS_YIELD:
            thread_yield();
            break;
    }
}

void check_address(const void *vaddr) {
    if (!is_user_vaddr(vaddr) || vaddr == NULL) {
        exit(-1);
    }
}

void halt(void) {
    shutdown_power_off();
}

void exit(int status) {
    struct thread *cur = thread_current();
    struct list_elem *e;

    cur->exitStatus = status;
    printf("%s: exit(%d)\n", cur->name, status);

    for (e = list_begin(&cur->child_list); e != list_end(&cur->child_list); e = list_next(e)) {
        struct thread *t = list_entry(e, struct thread, child);
        wait(t->tid);
    }

    for (int i = 0; i < 10; i++) {
        if (cur->save_signal[i] == NULL)
            break;
        free(cur->save_signal[i]);
    }
    thread_exit();
}

pid_t exec(const char *command) {
    char *file_name[128];
    memcpy(file_name, command, strlen(command) + 1);
    pid_t pid = process_execute(file_name);

    return pid;
}

int wait(pid_t pid) {
    return process_wait(pid);
}

bool create(const char *file, unsigned initial_size) {
    if (!file)
        exit(-1);
    lock_acquire(&filesys_lock);
    bool success = filesys_create(file, initial_size);
    lock_release(&filesys_lock);
    return success;
}

bool remove(const char *file) {
    lock_acquire(&filesys_lock);
    bool success = filesys_remove(file);
    lock_release(&filesys_lock);
    return success;
}

int open(const char *file) {
    struct thread *thread = thread_current();
    int fd = -1;

    if (!file)
        exit(-1);

    lock_acquire(&filesys_lock);  ////lock

    struct file *open_file = filesys_open(file);
    if (open_file != NULL) {
        int next_fd = thread->next_fd;
        if (next_fd >= 2 && next_fd < 256) {
            if (strcmp(thread->name, file) == 0)
                file_deny_write(open_file);
            thread->fdt[next_fd] = open_file;
            thread_current()->next_fd = next_fd + 1;
            fd = next_fd;
        }
    }

    lock_release(&filesys_lock);  ////lock

    return fd;
}

int filesize(int fd) {
    struct file *file = thread_current()->fdt[fd];
    if (file == NULL)
        return -1;
    off_t length = file_length(file);
    return length;
}

int read(int fd, void *buffer, unsigned size) {
    check_address(buffer);
    int return_val = 0;
    lock_acquire(&filesys_lock);

    if (fd == 0) {
        char ch;
        for (unsigned i = 0; i < size; i++) {
            ch = ((char *)buffer)[i] = input_getc();
            if (ch == '\n' || ch == '\0') {
                ((char *)buffer)[i] = '\0';
                break;
            } else {
                return_val++;
            }
        }
    } else {
        struct file *file = thread_current()->fdt[fd];
        if (file == NULL) {
            return_val = -1;
        } else {
            return_val = file_read(file, buffer, size);
        }
    }

    lock_release(&filesys_lock);
    return return_val;
}

int write(int fd, const void *buffer, unsigned size) {
    check_address(buffer);
    int return_val = -1;
    lock_acquire(&filesys_lock);

    if (fd == 1) {
        putbuf((const char *)buffer, size);
        return_val = size;
    } else {
        struct file *f_path = thread_current()->fdt[fd];
        if (f_path != NULL) {
            return_val = file_write(f_path, buffer, size);
        }
    }

    lock_release(&filesys_lock);
    return return_val;
}

void seek(int fd, unsigned position) {
    struct file *f_path = thread_current()->fdt[fd];
    if (f_path != NULL) {
        file_seek(f_path, position);
    }
}

unsigned tell(int fd) {
    struct file *f_path = thread_current()->fdt[fd];
    if (f_path == NULL) {
        return -1;
    }
    return file_tell(f_path);
}

void close(int fd) {
    if (fd < 2) return;
    struct file *f_path = thread_current()->fdt[fd];
    if (f_path == NULL) {
        return;
    }
    file_close(f_path);
    thread_current()->fdt[fd] = NULL;
}

void sched_yield(void) {
    thread_yield();
}

void sigaction(int signum, void (*handler)(void)) {
    struct thread *cur = thread_current();

    int i = 0;
    while (cur->save_signal[i] != NULL)
        i++;

    struct signal *sig_struct = (struct signal *)malloc(sizeof(struct signal));

    sig_struct->signum = signum;
    sig_struct->sig_handler = handler;
    (cur->save_signal[i]) = sig_struct;
}

void sendsig(pid_t pid, int signum) {
    sendsig_thread(pid, signum);
}
