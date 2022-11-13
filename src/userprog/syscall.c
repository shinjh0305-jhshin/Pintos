#include "userprog/syscall.h"

#include <stdio.h>
#include <syscall-nr.h>

#include "threads/interrupt.h"
#include "threads/thread.h"
// modified
#include "threads/synch.h"
#include "threads/vaddr.h"
// #include "userprog/syscall.h"

// #include <stdio.h>
// #include <string.h>
// #include <syscall-nr.h>

// /*Pintos 1_User program_include --------------------------------- STARTS HERE*/
// #include <list.h>

// #include "devices/input.h"
// #include "devices/shutdown.h"
// #include "threads/interrupt.h"
// #include "threads/synch.h"
// #include "threads/thread.h"
// #include "threads/vaddr.h"
// #include "userprog/pagedir.h"
// #include "userprog/process.h"
// /*Pintos 1_User program_include --------------------------------- ENDS HERE*/

// ////////////////////////////////////
// #include "filesys/directory.h"
// #include "filesys/file.h"
// #include "filesys/filesys.h"
// #include "filesys/inode.h"

static void syscall_handler(struct intr_frame *);

/*Pintos 1_User program_Check address, Get Argument --------------------------------- STARTS HERE*/
// esp가 유효한 주소인지 확인한다.
void check_address(const void *addr) {
    // check kernel addr
    if (addr == NULL || !is_user_vaddr(addr) /*|| (unsigned int *)addr < (unsigned int *)0x8048000*/) {
        exit(-1);
    }
}

// argument를 저장한다.
void getArgument(void *esp, int *arg, int count) {
    void *mov = esp;
    mov += 4;

    for (int i = 0; i < count; i++) {
        check_address(mov);
        arg[i] = *(int *)mov;
        mov += 4;
    }
}
/*Pintos 1_User program_Check address, Get Argument --------------------------------- ENDS HERE*/

void syscall_init(void) {
    lock_init(&filesys_lock);
    intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler(struct intr_frame *f UNUSED) {
    // printf("Entering syscall_handler.\n");
    int argv[4];
    check_address(f->esp);
    uint32_t syscallNum = *(int *)(f->esp);

    switch (syscallNum) {
        case SYS_HALT:
            halt();
            break;
        case SYS_EXIT:
            getArgument(f->esp, argv, 1);
            exit(argv[0]);
            break;
        case SYS_EXEC:
            getArgument(f->esp, argv, 1);
            check_address((void *)argv[0]);
            f->eax = exec((const char *)argv[0]);
            break;
        case SYS_WAIT:
            getArgument(f->esp, argv, 1);
            f->eax = wait(argv[0]);
            break;
        case SYS_CREATE:
            getArgument(f->esp, argv, 2);
            f->eax = create((const char *)argv[0], (unsigned)argv[1]);
            break;
        case SYS_REMOVE:
            getArgument(f->esp, argv, 1);
            f->eax = remove((const char *)argv[0]);
            break;
        case SYS_OPEN:
            getArgument(f->esp, argv, 1);
            f->eax = open((const char *)argv[0]);
            break;
        case SYS_FILESIZE:
            getArgument(f->esp, argv, 1);
            f->eax = filesize((int)argv[0]);
            break;
        case SYS_READ:
            getArgument(f->esp, argv, 3);
            f->eax = read((int)argv[0], (void *)argv[1], (unsigned)argv[2]);
            break;
        case SYS_WRITE:
            getArgument(f->esp, argv, 3);
            f->eax = write((int)argv[0], (void *)argv[1], (unsigned)argv[2]);
            break;
        case SYS_SEEK:
            getArgument(f->esp, argv, 2);
            seek((int)argv[0], (unsigned)argv[1]);
            break;
        case SYS_TELL:
            getArgument(f->esp, argv, 1);
            f->eax = tell((int)argv[0]);
            break;
        case SYS_CLOSE:
            getArgument(f->esp, argv, 1);
            close((int)argv[0]);
            break;
        case SYS_SIGACTION:
            getArgument(f->esp, argv, 2);
            sigaction((int)argv[0], (void *)argv[1]);
            break;
        case SYS_SENDSIG:
            getArgument(f->esp, argv, 2);
            sendsig((tid_t)argv[0], (int)argv[1]);
            break;
        case SYS_YIELD:
            thread_yield();
            break;
        default:
            break;
    }
}

/*Pintos 1_User program_exec, wait, exit, halt, read, write--------------------------------- STARTS HERE*/
tid_t exec(const char *task) {
    char *file_name[128];
    memcpy(file_name, task, strlen(task) + 1);
    tid_t child_tid = process_execute(file_name);

    return child_tid;
}

void exit(int status) {
    struct thread *thread = thread_current();
    thread->exitStatus = status;

    printf("%s: exit(%d)\n", thread->name, status);

    struct list_elem *e;
    struct list_elem *begin = list_begin(&(thread->child_list));
    struct list_elem *end = list_end(&(thread->child_list));
    for (e = begin; e != end; e = list_next(e)) {
        struct thread *t = list_entry(e, struct thread, child);
        wait(t->tid);
    }

    for (int i = 0; i < 10; i++) {
        if (thread->save_signal[i] == NULL)
            break;
        free(thread->save_signal[i]);
    }
    thread_exit();
}

bool create(const char *file, unsigned sz) {
    if (file == NULL)
        exit(-1);
    return filesys_create(file, sz);
}

bool remove(const char *file) {
    if (file == NULL)
        exit(-1);
    return filesys_remove(file);
}

int open(const char *file) {
    struct thread *thread = thread_current();

    if (file == NULL)
        exit(-1);
    lock_acquire(&filesys_lock);
    struct file *open_file = filesys_open(file);
    lock_release(&filesys_lock);
    if (open_file == NULL) {
        return -1;
    } else {
        int next_fd = thread->next_fd;
        if (next_fd >= 2 && next_fd < 128) {
            if (strcmp(thread->name, file) == 0)
                file_deny_write(open_file);
            thread->fdt[next_fd] = open_file;
            thread_current()->next_fd = next_fd + 1;
            return next_fd;
        }
    }
    return -1;
}

int filesize(int fd) {
    struct thread *thread = thread_current();
    struct file *file = thread->fdt[fd];

    if (file == NULL)
        return -1;
    return file_length(file);
}

int wait(tid_t tid) {
    return process_wait(tid);
}

void halt() {
    shutdown_power_off();
}

int read(int fd, void *buffer, unsigned size) {
    check_address(buffer);
    lock_acquire(&filesys_lock);
    int len = 0;
    if (fd == 0) {
        char ch;
        for (unsigned i = 0; i < size; i++) {
            ch = ((char *)buffer)[i] = input_getc();
            if (ch == '\n' || ch == '\0') {
                ((char *)buffer)[i] = '\0';
                break;
            } else {
                len++;
            }
        }
    } else {
        struct file *file = thread_current()->fdt[fd];
        if (file == NULL) {
            lock_release(&filesys_lock);
            return -1;
        }
        len = file_read(file, buffer, size);
    }
    lock_release(&filesys_lock);
    return len;
}

int write(int fd, const void *buffer, unsigned size) {
    lock_acquire(&filesys_lock);

    int ret = -1;
    if (fd == 1) {
        putbuf(buffer, size);
        ret = size;
    } else {
        struct file *file = thread_current()->fdt[fd];
        if (file == NULL) {
            lock_release(&filesys_lock);
            return -1;
        }
        ret = file_write(file, buffer, size);
    }

    lock_release(&filesys_lock);
    return ret;
}

void seek(int fd, unsigned pos) {
    struct file *file = thread_current()->fdt[fd];
    if (file == NULL) return;
    return file_seek(file, pos);
}

unsigned tell(int fd) {
    struct file *file = thread_current()->fdt[fd];
    if (file == NULL) return -1;
    return file_tell(file);
}

void close(int fd) {
    struct file *file = thread_current()->fdt[fd];
    if (file == NULL) return;
    return file_close(file);
    thread_current()->fdt[fd] = NULL;
}

void sched_yield() {
    thread_yield();
}

void sigaction(int signum, void (*handler)()) {
    struct thread *cur = thread_current();

    int i = 0;
    while (cur->save_signal[i] != NULL)
        i++;

    struct signal *sig_struct = (struct signal *)malloc(sizeof(struct signal));

    sig_struct->signum = signum;
    sig_struct->sig_handler = handler;
    (cur->save_signal[i]) = sig_struct;
}

void sendsig(tid_t pid, int signum) {
    sendsig_thread(pid, signum);
}
/*Pintos 1_User program_exec, wait, exit, halt, read, write--------------------------------- ENDS HERE*/
