//
// Created by dhanrajbir on 20/11/22.
//

#ifndef GAZPREABASE_STREAM_STATE_H
#define GAZPREABASE_STREAM_STATE_H

#define STREAM_BUF_LEN 1026


struct stream_state {
    char *write_head;
    char *read_head;
    char stream_status;
    char buf[STREAM_BUF_LEN];
};


#define STREAM_STATE_INITIALIZER {0, 0, 0, 0, 0};

typedef struct _stream_state {
    int pop_idx;
    char state;
    char buf[STREAM_BUF_LEN];
    char scan_buf[STREAM_BUF_LEN];
    char has_been_initialized;
} stream_state_t;

#endif //GAZPREABASE_STREAM_STATE_H
