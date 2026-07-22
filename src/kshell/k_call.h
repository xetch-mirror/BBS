#ifndef K_CALL_H
#define K_CALL_H

#include <stdint.h>

extern void switch_to_task(int next_task_id);
extern void sys_yield(void);
extern char sys_getchar(void);

#define kcall_yield()     sys_yield()
#define kcall_getchar()   sys_getchar()

#endif // K_CALL_H
