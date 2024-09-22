
#include <stdio.h>
#include "chsys/log.h"
#include "chsys/sys.h"

int main(void) {
    sys_init();

    log_info(true, "Here is a dynamic number %d", -134);
    log_warn(true, "Uh Oh, big time warning!");


    safe_exit(true, 0);
}
