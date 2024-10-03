
#ifndef CHSYS_MSG_H
#define CHSYS_MSG_H

#include <stdlib.h>
#include "chsys/mem.h"

// Code for passing messages between processes!

// UGH.. I'm tired...
// Don't really want to do anything today tbh...
// C'mon mate, you can do this... Put it some real effort!


// How should this all come together tbh??
// The pipe is constantly read from??
// It isn't constantly read from?
// We wait for the child process to die before reading from
// its pipe? Could be a better idea... what about pipe errors?
//
// We want non-blocking write with blocking read.. that's it.



typedef struct _msg_t {
    size_t len;
    void *data;
} msg_t;

static inline void delete_msg(msg_t *m) {
    safe_free(m->data); 
    safe_free(m);
}

static inline size_t m_len(const msg_t *m) {
    return m->len;
}

static inline void *m_data(const msg_t *m) {
    return m->data;
}

typedef struct _msg_queue_t {
    
} msg_queue_t;

// NOTE: a message queue expects messages to be in the format:
// | size_t len | byte data[len] .... |
//
// The message queue does store context of its position in processing
// data. When calling mq_interpret, it is possible the message is in the
// middle of some message.

void mq_interpret(const void *buf, size_t buf_len);

// The returned message will live in dynamic memory and should be deleted
// using delete_msg. The message owns its data. If you'd like to use it after
// deletion of the message, be sure to copy the data into a new buffer.
msg_t *mq_dequeue(void);


#endif
