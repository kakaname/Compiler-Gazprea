#include <ctype.h>
#include <limits.h>
#include "runtime.h"
#include "stream_state.h"


struct stream_store s = {0, 0, 0, 0, 0, {0}};


stream_state_t stream = STREAM_STATE_INITIALIZER;

static void consume_next_n(int n) {
    for (int i = 0; i < n; i++) {
        stream.buf[stream.pop_idx] = (char) getchar();
        stream.pop_idx = (stream.pop_idx + 1) % STREAM_BUF_LEN;
    }
}

static int put_data_in_read_buf() {
    int current_pop_idx = stream.pop_idx;
    int characters_read = 0;

    // Read in the spaces.
    while (characters_read < STREAM_BUF_LEN - 1 && isspace(stream.buf[current_pop_idx])) {
        stream.scan_buf[characters_read++] = stream.buf[current_pop_idx];
        current_pop_idx = (current_pop_idx + 1) % STREAM_BUF_LEN;
    }

    while (characters_read < STREAM_BUF_LEN - 1 && !isspace(stream.buf[current_pop_idx])) {
        stream.scan_buf[characters_read++] = stream.buf[current_pop_idx];
        current_pop_idx = (current_pop_idx + 1) % STREAM_BUF_LEN;
    }

    if (characters_read > 1024) {
        fprintf(stderr, "Had to look ahead more than kB of input to find next token");
        exit(1);
    }

    stream.scan_buf[characters_read] = 0;
    return characters_read;
}

int64_t stream_state() {
    return stream.state;
}

void rt_stream_in_init() {
    for (int i = 0; i < STREAM_BUF_LEN; i++)
        stream.buf[i] = getchar();
}

static char has_input_ended() {
    return stream.buf[stream.pop_idx] == EOF;
}

void rt_print_matrix(struct matrix *m) {
    printf("[");
    struct vector **data = m->data;
    if (m->type == VECTOR_TYPE_INT) {
        for (int64_t i = 0; i < m->rows; i++) {
            printf("[");
            for (int64_t j = 0; j < m->cols; j++) {
                printf("%ld", ((int64_t *) data[i]->data )[j]);
                if (j != m->cols - 1) {
                    printf(" ");
                }
            }
            printf("]");
            if (i != m->rows - 1) {
                printf(" ");
            }
        }
    } else if (m->type == VECTOR_TYPE_FLOAT) {
        for (int64_t i = 0; i < m->rows; i++) {
            printf("[");
            for (int64_t j = 0; j < m->cols; j++) {
                printf("%g", ((float *) data[i]->data )[j]);
                if (j != m->cols - 1) {
                    printf(" ");
                }
            }
            printf("]");
            if (i != m->rows - 1) {
                printf(" ");
            }
        }
    } else if (m->type == VECTOR_TYPE_CHAR) {
        for (int64_t i = 0; i < m->rows; i++) {
            printf("[");
            for (int64_t j = 0; j < m->cols; j++) {
                printf("%c", ((char *) data[i]->data )[j]);
                if (j != m->cols - 1) {
                    printf(" ");
                }
            }
            printf("]");
            if (i != m->rows - 1) {
                printf(" ");
            }
        }
    } else if (m->type == VECTOR_TYPE_BOOL) {
        for (int64_t i = 0; i < m->rows; i++) {
            printf("[");
            for (int64_t j = 0; j < m->cols; j++) {
                printf("%c", ((char *) data[i]->data )[j] ? 'T' : 'F');
                if (j != m->cols - 1) {
                    printf(" ");
                }
            }
            printf("]");
            if (i != m->rows - 1) {
                printf(" ");
            }
        }
    }
    printf("]");
}

void rt_print_vector(struct vector *v) {
    printf("[");
    if (v->type == VECTOR_TYPE_INT) {
        int64_t *data = (int64_t *) v->data;
        for (int64_t i = 0; i < v->size; i++) {
            printf("%ld", data[i]);
            if (i != v->size - 1)
                printf(" ");
        }
    } else if (v->type == VECTOR_TYPE_FLOAT) {
        float *data = (float *) v->data;
        for (int64_t i = 0; i < v->size; i++) {
            printf("%g", data[i]);
            if (i != v->size - 1)
                printf(" ");
        }
    } else if (v->type == VECTOR_TYPE_CHAR) {
        char *data = (char *) v->data;
        for (int64_t i = 0; i < v->size; i++) {
            printf("%c", data[i]);
            if (i != v->size - 1)
                printf(" ");
        }
    } else if (v->type == VECTOR_TYPE_BOOL) {
        char *data = (char *) v->data;
        for (int64_t i = 0; i < v->size; i++) {
            printf("%c", data[i] ? 'T' : 'F');
            if (i != v->size - 1)
                printf(" ");
        }
    }
    printf("]");
}

void rt_print_string(struct vector *v) {
    char *data = (char *) v->data;
    for (int i = 0; i < v->size; i++) {
        printf("%c", data[i]);
    }
}

void rt_print_int(int64_t i) {
    printf("%ld", i);
}

void rt_print_real(float d) {
    printf("%g", d);
}

void rt_print_char(char c) {
    printf("%c", c);
}

void rt_print_bool(int64_t b) {
    printf("%c", b ? 'T' : 'F');
}

char rt_scan_char() {
    // If the stream has ended we just set the state and return -1 for
    // the character.
    if (has_input_ended()) {
        stream.state = 2;
        return -1;
    }

    char char_from_buf = stream.buf[stream.pop_idx];
    consume_next_n(1);
    return char_from_buf;
}

char rt_scan_bool() {

    if (has_input_ended()) {
        stream.state = 2;
        return 0;
    }

    int buf_len = put_data_in_read_buf();
    char bool;
    int consumed;
    sscanf(stream.scan_buf, " %c%n", &bool, &consumed);

    // There are some extra characters in the buffer.
    if (consumed != buf_len) {
        stream.state = 1;
        return 0;
    }

    if (bool != 'T' && bool != 'F') {
        stream.state = 1;
        return 0;
    }

    consume_next_n(buf_len);
    return bool == 'T';
}

int64_t rt_scan_int() {

    if (has_input_ended()) {
        stream.state = 2;
        return 0;
    }

    int buf_len = put_data_in_read_buf();
    int64_t read_in_int;
    int consumed;
    if (!sscanf(stream.scan_buf, " %ld%n", &read_in_int, &consumed)) {
        stream.state = 1;
        return 0;
    }

    // There are some extra characters in the buffer.
    if (consumed != buf_len) {
        stream.state = 1;
        return 0;
    }

    char *without_ws = stream.scan_buf;

    while (isspace(without_ws))
        ++without_ws;


    // If without space, the buf len is more than 11, then obviously its an error
    // as the min int32 takes 11 characters to represent.
    if (strlen(without_ws) > 11) {
        stream.state = 1;
        return 0;
    }

    if (read_in_int < INT_MIN || read_in_int > INT_MAX) {
        stream.state = 1;
        return 0;
    }

    consume_next_n(buf_len);
    return read_in_int;
}

float rt_scan_real() {
    if (has_input_ended()) {
        stream.state = 2;
        return 0.0f;
    }

    int buf_len = put_data_in_read_buf();

    float read_in_float;
    int consumed;

    if (!sscanf(stream.scan_buf, " %f%n", &read_in_float, &consumed)) {
        stream.state = 1;
        return 0.0f;
    }

    if (consumed != buf_len) {
        stream.state = 1;
        return 0.0f;
    }

    consume_next_n(buf_len);
    return read_in_float;
}

