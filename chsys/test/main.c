
#include <signal.h>
#include <stdio.h>
#include "chsys/log.h"
#include "chsys/mem.h"
#include "chsys/sys.h"
#include <stdlib.h>
#include <unistd.h>
#include "sys.h"

// We won't have UNITY tests here.
// Just some general tests that multiprocessing is working as
// expected.

int main(void) {
    run_sys_tests();
}
