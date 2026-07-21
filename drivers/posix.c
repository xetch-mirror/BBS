#include <stdint.h>
#include <stddef.h>

#define SYS_READ      0
#define SYS_WRITE     1
#define SYS_OPENAT    257
#define SYS_CLOSE     3
#define SYS_MMAP      9
#define SYS_MUNMAP    11
#define SYS_ARCH_PRCTL 158
#define SYS_EXIT      60

int64_t posix_syscall_handler(uint64_t sys_num, uint64_t arg1, uint64_t arg2, 
                              uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    switch (sys_num) {
        case SYS_WRITE:
            // arg1 = fd, arg2 = buf, arg3 = count
            return sys_posix_write((int)arg1, (const char*)arg2, (size_t)arg3);

        case SYS_READ:
            return sys_posix_read((int)arg1, (char*)arg2, (size_t)arg3);

        case SYS_MMAP:
            // malloc
            return (int64_t)sys_posix_mmap((void*)arg1, (size_t)arg2, (int)arg3);

        case SYS_EXIT:
            sys_posix_exit((int)arg1);
            return 0; // Unreachable

        default:
            // I severed a filesystem link and I'm lying to c
            return -38; 
    }
}
