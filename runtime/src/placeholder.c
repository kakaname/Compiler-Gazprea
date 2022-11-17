#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"

struct stream_store {
    size_t stream_state;
    size_t front; // Front of queue
    size_t back;  // Back of queue
    size_t curr;  // Current position when "peeking"
    size_t eof;
    char buffer[1025];
};

// TODO test limits of integer

void rt_print_int(int i) {
    printf("%d", i);
}

void rt_print_real(float d) {
    printf("%g", d);
}

void rt_print_char(char c) {
    printf("%c", c);
}

void rt_print_bool(int b) {
    printf("%c", b ? 'T' : 'F');
}

void queue_char(struct stream_store *s, char c) {
    // check if the buffer is full
    if ((s->back + 1) % 1025 == s->front) {
        ERR_BUFFER_OVERFLOW();
    } else {
        s->buffer[s->back] = c;
        s->back = (s->back + 1) % 1025;
    }
}

char dequeue_char(struct stream_store *s) {
    // check if the buffer is empty
    if (s->front == s->back) {
        ERR_BUFFER_OVERFLOW();
    } else {
        char c = s->buffer[s->front];
        s->front = (s->front + 1) % 1025;
        return c;
    }
}

char dequeue_curr_char(struct stream_store *s) {
    // check if the buffer is empty
    if (s->curr == s->back) {
        ERR_BUFFER_OVERFLOW();
    } else {
        char c = s->buffer[s->curr];
        s->curr = (s->curr + 1) % 1025;
        return c;
    }
}


char consume_char(struct stream_store *s) {
    char c;
    if (s->back + 1 == s->front) ERR_BUFFER_OVERFLOW();
    // check if the buffer is empty
    if (s->front == s->back) {
        // read from the stream
        if (s->eof) { return -1; }
        if (scanf("%c", &c) == EOF) {
            s->eof = 1;
            c = -1;
        }
        queue_char(s, c);
        s->front = s->back;
    } else {
        // read from the buffer
        c = dequeue_char(s);
    }

    return c;
}

char is_ws(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}


char* peek_token(struct stream_store *s, char eof_ss) {
    char c;
    s->curr = s->front;
    size_t ws_start = s->front;
    // check if the buffer is empty
    while (1) {
        if (s->curr == s->back) {
            // read from the stream
            if (s->back + 1 == s->front) ERR_BUFFER_OVERFLOW();
            if (scanf("%c", &c) == EOF) {
                if (s->eof) {
                    s->stream_state = eof_ss;
                    return NULL;
                }
                s->eof = 1;
                c = 0;
            }
            queue_char(s, c);
            s->curr = s->back;
        } else {
            // read from the buffer
            c = dequeue_curr_char(s);
        }
        size_t len = s->front > s->back ? (1024 - s->front + s->back) : (s->back - s->front);
        if ((is_ws(c) || s->eof) && len > 1 && !is_ws(s->buffer[(s->curr - 2) % 1025])) break;
        if (is_ws(c)) ws_start = s->curr;
    }

    // calculate size of token
    size_t bufsize = ws_start > s->back ? (1024 - ws_start + s->back + 1) : (s->back - ws_start - 1) + 1;
//    if (is_ws(c)) bufsize--;

    // copy the token into a new buffer
    char *token = malloc(bufsize + 1);
    size_t i = 0;
    while (i != bufsize) {
        token[i] = s->buffer[ws_start];
        ws_start = (ws_start + 1) % 1025;
        i++;
    }

    token[i-1] = '\0';
    return token;
}

void consume_to_curr(struct stream_store *s) {
    // set front of buffer to current position
    s->front = (s->curr - 1) % 1025;
}

void consume_single(struct stream_store *s) {
    // increment front of buffer
    s->front = (s->front + 1) % 1025;
}

char peek_next(struct stream_store *s) {
    char c = ' ';
    // While the current char is whitespace, we have not reached the end of file, or
    while (is_ws(c) && !s->eof && (s->back + 1) % 1025 != s->front) {
        // check if the buffer is empty
        if (s->curr == s->back) {
            // read from the stream
            if (scanf("%c", &c) == EOF) {
                s->eof = 1;
            }
            // put the character back
            s->buffer[s->back] = c;
            s->back = (s->back + 1) % 1025;
        } else {
            // read from the buffer
            c = s->buffer[s->curr];
        }
        s->curr = (s->curr + 1) % 1025;
    }

    if (s->eof) {
        return EOF;
    } else {
        return c;
    }
}

char peek_char(struct stream_store *s) {
    // reset peek to beginning
    s->curr = s->front;
    return peek_next(s);

}

void rt_scan_char(char *c, struct stream_store *s) {
    // TODO fix one missing after final
    char temp = consume_char(s);
    s->stream_state = 0;
    *c = temp;
}

void rt_scan_bool(int *b, struct stream_store *s) {
    char c = peek_char(s);
    if (c == 'T') {
        consume_to_curr(s);
        s->stream_state = 0;
        *b = 1;
    } else if (c == 'F') {
        consume_to_curr(s);
        s->stream_state = 0;
        *b = 0;
    } else if (c == EOF) {
        consume_single(s);
        s->stream_state = 1;
        *b = 0;
    } else {
        consume_single(s);
        s->stream_state = 2;
        *b = 0;
    }
}

void scan_int_err(struct stream_store *s, int *i, char val) {
    s->stream_state = val;
    *i = 0;
}

void rt_scan_int(int *i, struct stream_store *s) {

    char *token = peek_token(s, 2);
    if (token == NULL) return scan_int_err(s, i, 2);

    // check that there are only numbers and sign in the token
    size_t str_len = strlen(token);
    int j = 0;
    if (token[0] == '-' || token[0] == '+') j = 1;

    if (str_len == j) return scan_int_err(s, i, 1);
    for (; j < strlen(token); j++)
        if (token[j] < '0' || token[j] > '9') return scan_int_err(s, i, 1);

    int num = atoi(token);
    s->stream_state = 0;
    *i = num;

    free(token);
    consume_to_curr(s);
}

void rt_scan_real(float *d, struct stream_store *s) {
    int seen_dot = 0;
    char *token = peek_token(s, 2);
    if (token == NULL) return scan_int_err(s, d, 2);

    // check that there are only numbers and sign in the token
    size_t str_len = strlen(token);
    int j = 0;
    if (token[0] == '-' || token[0] == '+') j = 1;

    if (str_len == j) return scan_int_err(s, d, 1);
    for (; j < strlen(token); j++) {
        if (token[j] == '.') {
            if (seen_dot) return scan_int_err(s, d, 1);
            seen_dot = 1;
        } else if (token[j] < '0' || token[j] > '9') return scan_int_err(s, d, 1);
    }

    float num = atof(token);
    s->stream_state = 0;
    *d = num;

    free(token);
    consume_to_curr(s);
}


// sample main function for testing

int main() {

    struct stream_store s = {0};

    int i;
    float d;
    char c = 0;
    int b;

    while (1) {
        rt_scan_real(&d, &s);
        printf("real: %g, state: %d\n", d, s.stream_state);

        if (s.stream_state != 0) break;
    }

    return 0;
}
