#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "def.h"

int64 sys_tick_count(void);
int sys_cpu_count(void);
void sys_abort(const char *fmt, ...);
void *sys_malloc(size_t size);
void *sys_realloc(void *ptr, size_t size);
void sys_free(void *ptr);
void *sys_aligned_malloc(size_t size, size_t alignment);
void sys_aligned_free(void *ptr);

#endif //SYSTEM_H_
