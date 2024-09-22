
#ifndef CHSYS_SYS_H
#define CHSYS_SYS_H

#include <stdlib.h>
#include <stdbool.h>

// An error during setup will result in exit.
// A message will be printed, but not in a pretty way!
void sys_init(void);

// The system state contains a lock.
// You can aquire/release said lock using these calls!
// If given acquire lock is false, these two below functions are no-ops.
void sys_lock(bool acquire_lock);
void sys_unlock(bool acquire_lock);

void sys_set_quiet(bool acquire_lock, bool q);
bool sys_is_quiet(bool acquire_lock);

void sys_inc_malloc_count(bool acquire_lock);
void sys_dec_malloc_count(bool acquire_lock);
size_t sys_get_malloc_count(bool acquire_lock);
void sys_reset_malloc_count(bool acquire_lock);

// These below calls are all threadsafe!
pid_t safe_fork(void);
pid_t safe_waitpid(pid_t pid, int *wstatus, int options);

void safe_exit(bool acquire_lock, int status);

#endif
