
#include "chsys/log.h"
#include "chsys/sys.h"

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/_types/_va_list.h>
#include <unistd.h>

typedef struct _log_level_style_t {
    const char *label;
    const char *label_style;
    const char *msg_style;
} log_level_style_t;

static const log_level_style_t LOG_LEVEL_TO_STYLE[] = {
    {
        .label = "INFO",
        .label_style = ANSI_BOLD ANSI_BRIGHT_BLUE_FG,
        .msg_style = ANSI_ITALIC ANSI_BLUE_FG,
    },
    {
        .label = "WARN",
        .label_style = ANSI_BOLD ANSI_BRIGHT_YELLOW_FG,
        .msg_style = ANSI_ITALIC ANSI_YELLOW_FG,
    },
    {
        .label = "FATAL",
        .label_style = ANSI_BOLD ANSI_BRIGHT_RED_FG,
        .msg_style = ANSI_ITALIC ANSI_RED_FG,
    },
};

// Call this when you have the system lock
// AND you aren't in quiet mode!
static void _log_any(sys_log_level_t level, const char *fmt, va_list args) {
    // First let's log our pid.
    pid_t pid = getpid();
    printf(ANSI_BOLD "(" ANSI_RESET);
    printf(ANSI_BRIGHT_CYAN_FG "%d" ANSI_RESET, pid);
    printf(ANSI_BOLD  ") " ANSI_RESET);

    const log_level_style_t *style = &(LOG_LEVEL_TO_STYLE[level]);

    printf("%s%s%s ", style->label_style, style->label, ANSI_RESET);
    
    printf("%s", style->msg_style);
    vprintf(fmt, args);
    printf(ANSI_RESET "\n");
}

void log_any(bool acquire_lock, sys_log_level_t level, const char *fmt,...) {
    sys_lock(acquire_lock);
    if (!sys_is_quiet(false)) {
        va_list args;
        va_start(args, fmt);
        _log_any(level, fmt, args); 
        va_end(args);
    }
    if (level == SYS_FATAL) {
        safe_exit(false, 1);
    }
    sys_unlock(acquire_lock);
}





