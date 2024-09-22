
#include "chsys/sys.h"
#include "chsys/mem.h"
#include "chsys/log.h"

#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>

// mean to only be used during setup.
#define ERROR_OUT(...) \
    do {\
        printf(__VA_ARGS__);\
        exit(1);\
    } while (0)

// Core linked list for holding children.
typedef struct _child_node_t {
    pid_t pid;
    struct _child_node_t *next;
} child_node_t;

typedef struct _sys_state_t {
    pthread_mutex_t lock;

    bool quiet;
    size_t malloc_count;
    child_node_t *child_list;
} sys_state_t;

static sys_state_t *ss = NULL;

static void *sig_thread(void *arg) {
    (void)arg;

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    int sig; // Not really needed, unsure if you can pass NULL
             // into sigwait tho.
    int s;

    // Wait for just one SIGINT.
    s = sigwait(&set, &sig);     
    if (s) {
        log_fatal(true, "Error from sigwait. (%d)", s);
    }

    sys_lock(true);
    log_info(false, "SIGINT received.");
    safe_exit(false, 0);
    sys_unlock(true);

    // Will never make it here.
    return NULL;
}

void sys_init(void) {
    int s;

    if (ss) {
        // We can call log here since our system is already setup!
        log_fatal(true, "System attempting to be init'd more than once.");
    }

    // Let's setup our state in memory.
    ss = malloc(sizeof(sys_state_t));
    if (!ss) {
        ERROR_OUT("Could not malloc system state\n");
    }

    ss->quiet = 0;
    ss->malloc_count = 0;
    ss->child_list = NULL;

    s = pthread_mutex_init(&(ss->lock), NULL);
    if (s) {
        ERROR_OUT("Could not init system state mutex\n");
    }

    // We've initialized our system state!
    // Now we can call log!

    // All threads will block SIGINT.
    // The signal thread will be the only one which actually
    // services the pending signal.
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    s = pthread_sigmask(SIG_BLOCK, &set, NULL);
    if (s) {
        log_fatal(true, "Failed to block SIGINT. (%d)", s);
    }

    // Spawn our signal handling thread. 
    // (THIS SHOULD ALWAYS BE THE LAST ACTION OF THIS FUNCITON)
    pthread_t st;
    s = pthread_create(&st, NULL, sig_thread, NULL);
    if (s) {
        log_fatal(true, "Failed to create signal handling thread. (%d)", s);
    }
}

void sys_lock(bool acquire_lock) {
    if (acquire_lock) {
        pthread_mutex_lock(&(ss->lock));
    }
}

void sys_unlock(bool acquire_lock) {
    if (acquire_lock) {
        pthread_mutex_unlock(&(ss->lock));
    }
}

void sys_set_quiet(bool acquire_lock, bool q) {
    sys_lock(acquire_lock);
    ss->quiet = q;
    sys_unlock(acquire_lock);
}

bool sys_is_quiet(bool acquire_lock) {
    bool res;

    sys_lock(acquire_lock);
    res = ss->quiet;
    sys_unlock(acquire_lock);

    return res;
}

void sys_inc_malloc_count(bool acquire_lock) {
    sys_lock(acquire_lock);
    if (ss->malloc_count == SIZE_T_MAX) {
        log_fatal(false, "Malloc overflow");
    }
    ss->malloc_count++;
    sys_unlock(acquire_lock);
}

void sys_dec_malloc_count(bool acquire_lock) {
    sys_lock(acquire_lock);
    if (ss->malloc_count == 0) {
        log_fatal(false, "Malloc underflow");
    }
    ss->malloc_count--;
    sys_unlock(acquire_lock);
}

size_t sys_get_malloc_count(bool acquire_lock) {
    size_t mc;

    sys_lock(acquire_lock);
    mc = ss->malloc_count;
    sys_unlock(acquire_lock);

    return mc;
}

void sys_reset_malloc_count(bool acquire_lock) {
    sys_lock(acquire_lock);
    ss->malloc_count = 0;
    sys_unlock(acquire_lock);
}

// If kill and wait is given, a SIGKILL will be sent to each
// child, and the child will be waited on.
/*
static void free_child_list(bool killAndWait) {
    child_node_t *iter;
    child_node_t *next;

    iter = ss->child_list;
    while (iter) {
        if (killAndWait) {
            kill(iter->pid, SIGINT);
            waitpid(iter->pid, NULL, 0);
        }

        next = iter->next;
        free(iter);
        iter = next;
    }

    ss->child_list = NULL;
}

pid_t safe_fork(void) {
    pid_t pid = fork();
    
    if (pid < 0) {
        safe_exit(1);
    }

    if (pid == 0) {
        // Free the child list without killing/waiting.
        free_child_list(false);
        return 0;
    }

    // parent process.
    child_node_t *node = malloc(sizeof(child_node_t));

    if (!node) {
        // Since our new child was never put in the list...
        // gotta kill/wait on it here manually.
        kill(pid, SIGINT);
        waitpid(pid, NULL, 0);

        safe_exit(1);
    }

    node->pid = pid;
    node->next = ss->child_list;
    ss->child_list = node;
    
    return pid;
}

pid_t safe_waitpid(pid_t pid, int *wstatus, int options) {
    pid_t p = waitpid(pid, wstatus, options);

    if (p < 0) {
        safe_exit(1);
    }

    // WNOHANG case.
    if (p == 0) {
        return 0;
    }

    // Actual process reaped!
    child_node_t *prev = NULL;
    child_node_t *iter = ss->child_list;

    // If there's a process to wait on, it must appear in
    // the child list... thus, this loop must exit!
    while (1) {
        if (iter->pid == p) {
            if (prev) {
                prev->next = iter->next;
            } else {
                ss->child_list = iter->next;
            }

            free(iter);
            return p;
        }

        // Advance.
        prev = iter;
        iter = prev->next;
    }
}
*/

void safe_exit(bool acquire_lock, int status) {
    (void)acquire_lock;
    exit(status);
}
