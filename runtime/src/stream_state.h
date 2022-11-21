//
// Created by dhanrajbir on 20/11/22.
//

#ifndef GAZPREABASE_STREAM_STATE_H
#define GAZPREABASE_STREAM_STATE_H

#define STREAM_BUF_LEN 1025


struct stream_state {
    char *write_head;
    char *read_head;
    char stream_status;
    char buf[STREAM_BUF_LEN];
};

#endif //GAZPREABASE_STREAM_STATE_H
