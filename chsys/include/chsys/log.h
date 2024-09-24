
#ifndef CHSYS_LOG_H
#define CHSYS_LOG_H

#include <stdbool.h>

#define ANSI_CNTRL(code_str) "\x1B[" code_str "m"

#define ANSI_RESET      ANSI_CNTRL("0")
#define ANSI_BOLD       ANSI_CNTRL("1")
#define ANSI_ITALIC     ANSI_CNTRL("3")
#define ANSI_UNDERLINE  ANSI_CNTRL("4")

#define ANSI_BLACK_FG       ANSI_CNTRL("30")
#define ANSI_RED_FG         ANSI_CNTRL("31")
#define ANSI_GREEN_FG       ANSI_CNTRL("32")
#define ANSI_YELLOW_FG      ANSI_CNTRL("33")
#define ANSI_BLUE_FG        ANSI_CNTRL("34")
#define ANSI_MAGENTA_FG     ANSI_CNTRL("35")
#define ANSI_CYAN_FG        ANSI_CNTRL("36")
#define ANSI_WHITE_FG       ANSI_CNTRL("37")

#define ANSI_BLACK_BG       ANSI_CNTRL("40")
#define ANSI_RED_BG         ANSI_CNTRL("41")
#define ANSI_GREEN_BG       ANSI_CNTRL("42")
#define ANSI_YELLOW_BG      ANSI_CNTRL("43")
#define ANSI_BLUE_BG        ANSI_CNTRL("44")
#define ANSI_MAGENTA_BG     ANSI_CNTRL("45")
#define ANSI_CYAN_BG        ANSI_CNTRL("46")
#define ANSI_WHITE_BG       ANSI_CNTRL("47")

#define ANSI_BRIGHT_BLACK_FG       ANSI_CNTRL("90")
#define ANSI_BRIGHT_RED_FG         ANSI_CNTRL("91")
#define ANSI_BRIGHT_GREEN_FG       ANSI_CNTRL("92")
#define ANSI_BRIGHT_YELLOW_FG      ANSI_CNTRL("93")
#define ANSI_BRIGHT_BLUE_FG        ANSI_CNTRL("94")
#define ANSI_BRIGHT_MAGENTA_FG     ANSI_CNTRL("95")
#define ANSI_BRIGHT_CYAN_FG        ANSI_CNTRL("96")
#define ANSI_BRIGHT_WHITE_FG       ANSI_CNTRL("97")

#define ANSI_BRIGHT_BLACK_BG       ANSI_CNTRL("100")
#define ANSI_BRIGHT_RED_BG         ANSI_CNTRL("101")
#define ANSI_BRIGHT_GREEN_BG       ANSI_CNTRL("102")
#define ANSI_BRIGHT_YELLOW_BG      ANSI_CNTRL("103")
#define ANSI_BRIGHT_BLUE_BG        ANSI_CNTRL("104")
#define ANSI_BRIGHT_MAGENTA_BG     ANSI_CNTRL("105")
#define ANSI_BRIGHT_CYAN_BG        ANSI_CNTRL("106")
#define ANSI_BRIGHT_WHITE_BG       ANSI_CNTRL("107")

typedef enum _sys_log_level_t {
    SYS_INFO = 0,
    SYS_WARN,
    SYS_FATAL
} sys_log_level_t;

// If aquire lock is true, this call will aquire the system lock before printing.
// Otherwise, it won't.
void log_any_p(bool aquire_lock, sys_log_level_t level, const char *fmt,...);

#define log_info_p(al,...)   log_any_p(al,SYS_INFO,__VA_ARGS__)
#define log_warn_p(al,...)   log_any_p(al,SYS_WARN,__VA_ARGS__)
#define log_fatal_p(al,...)  log_any_p(al,SYS_FATAL,__VA_ARGS__)

#define log_info(...)   log_any_p(true,SYS_INFO,__VA_ARGS__)
#define log_warn(...)   log_any_p(true,SYS_WARN,__VA_ARGS__)
#define log_fatal(...)  log_any_p(true,SYS_FATAL,__VA_ARGS__)

#endif
