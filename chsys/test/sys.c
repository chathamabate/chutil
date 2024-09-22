
#include "chsys/sys.h"
#include "chsys/log.h"
#include "chsys/mem.h"
#include "sys.h"
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

static void test_init_and_exit(void) {
    sys_init(); 
    safe_exit(true, 0);
}

static void test_mem_leak(void) {
    sys_init(); 
    safe_malloc(100);
    safe_exit(true, 0);
}

static void test_no_mem_leak(void) {
    sys_init();
    void *mem = safe_malloc(100);
    safe_free(mem);
    safe_exit(true, 0);
}

static void test_sigint_catch(void) {
    sys_init();

    while (true) {
        sleep(1);
        log_info(true, "Send a SIGINT to kill me.");
    }
}

static void test_simple_fork(void) {
    sys_init();

    pid_t p = safe_fork();
    if (p == 0) {
        while (true) {
            sleep(1);
            log_info(true, "Hello from child process.");
        }
    }

    sleep(3);
    safe_exit(true, 0);
}

static void test_multi_fork(void) {
    sys_init();
    for (size_t i = 0; i < 4; i++) {
        pid_t p = safe_fork();

        if (p == 0) {
            while (true) {
                sleep(1);
                log_info(true, "Hello from child process %zu.", i);
            }
        }
    }

    sleep(3);
    safe_exit(true, 0);
}

static void test_nested_forks(void) {
    sys_init();
    pid_t p = safe_fork();
    if (p == 0) {
        safe_fork();
        safe_fork();
        while (true) {
            sleep(1);
            log_info(true, "Hello");
        }
    }

    sleep(3);
    safe_exit(true, 0);
}

static void test_waitpid(void) {
    sys_init();

    pid_t p = safe_fork();
    if (p == 0) {
        while (true);
    }

    kill(p, SIGINT);
    safe_waitpid(p, NULL, 0);

    // Spawn 2 children.
    for (size_t i = 0; i < 2; i++) {
        if (safe_fork() == 0) {
            while (true);
        }
    }

    p = safe_fork();
    if (p == 0) {
        while (true);
    }

    // Spawn 2 more children.
    for (size_t i = 0; i < 2; i++) {
        if (safe_fork() == 0) {
            while (true);
        }
    }

    // kill middle guy.
    kill(p, SIGINT);
    safe_waitpid(p, NULL, 0);

    // Expect 4 kills on exit.

    safe_exit(true, 0);
}


void run_sys_tests(void) {
    (void)test_init_and_exit;
    //test_init_and_exit();
    
    (void)test_mem_leak;
    //test_mem_leak();
    
    (void)test_no_mem_leak;
    //test_no_mem_leak();
    
    (void)test_sigint_catch;
    //test_sigint_catch();

    (void)test_simple_fork;
    //test_simple_fork();

    (void)test_multi_fork;
    //test_multi_fork();

    (void)test_nested_forks;
    //test_nested_forks();

    (void)test_waitpid;
    //test_waitpid();
}
