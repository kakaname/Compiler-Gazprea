#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"

//#define POINTER_CAST (type, name, item) \
//    type *name = (type *) item;
//
struct stream_store {
    size_t front; // Front of queue
    size_t back;  // Back of queue
    size_t curr;  // Current position when "peeking"
    size_t eof;
    char stream_state;
    char buffer[1025];
};

enum vector_type {
    VECTOR_TYPE_BOOL = 0,
    VECTOR_TYPE_CHAR = 1,
    VECTOR_TYPE_INT = 2,
    VECTOR_TYPE_FLOAT = 3
};

struct vector {
    u_int32_t size;
    u_int32_t idx;
    u_int32_t type;
    void *data;
};

// NOT IN USE
struct vector vector_new(enum vector_type type, u_int32_t size) {
    struct vector v;
    v.size = size;
    v.idx = 0;
    v.type = type;
    switch (type) {
        case VECTOR_TYPE_BOOL:
        case VECTOR_TYPE_CHAR:
            // NOTE: booleans are stored as bytes
            v.data = malloc(sizeof(char) * size);
            break;
        case VECTOR_TYPE_INT:
            v.data = malloc(sizeof(int) * size);
            break;
        case VECTOR_TYPE_FLOAT:
            v.data = malloc(sizeof(float) * size);
            break;
        default:
            exit(1);
    }
    return v;
}

enum vector_arithmeic_op_kind {
    EXP = 0,
    MUL,
    DIV,
    MOD,
    ADD,
    SUB
};

#define VECTOR_OP(enum_name, op) \
    case enum_name:                  \
        res_data[i] = a_data[i] op b_data[i]; \
        break;

// TODO fix mod and power

#define ARITHMETIC_OPS \
switch (op) {          \
    VECTOR_OP(MUL, *)  \
    VECTOR_OP(DIV, /)  \
    VECTOR_OP(ADD, +)  \
    VECTOR_OP(SUB, -)  \
    default:           \
        exit(1);       \
}

#define VECTOR_A_INIT(type) \
    type *a_data = (type *) a->data;

#define VECTOR_RES_INIT(type) \
    type *res_data = (type *) res.data;

#define VECTOR_DATA_INIT_SINGLE(type) \
    VECTOR_A_INIT(type) \
    VECTOR_RES_INIT(type)

#define VECTOR_DATA_INIT(type) \
    VECTOR_A_INIT(type)        \
    type *b_data = (type *) b->data;

#define VECTOR_DATA_INIT_RES(type) \
    VECTOR_DATA_INIT(type)         \
    type *res_data = (type *) res.data;

struct vector rt_vector_not(struct vector *a) {
    struct vector res = vector_new(a->type, a->size);
    VECTOR_DATA_INIT_SINGLE(char)
    for (int i = 0; i < a->size; i++) {
        res_data[i] = !a_data[i];
    }
    return res;
}

struct vector rt_vector_sub(struct vector *a) {
    struct vector res = vector_new(a->type, a->size);
    if (a->type == VECTOR_TYPE_INT) {
        VECTOR_DATA_INIT_SINGLE(int)
        for (int i = 0; i < a->size; i++) {
            res_data[i] = -a_data[i];
        }
    } else if (a->type == VECTOR_TYPE_FLOAT) {
        VECTOR_DATA_INIT_SINGLE(float)
        for (int i = 0; i < a->size; i++) {
            res_data[i] = -a_data[i];
        }
    } else {
        exit(1);
    }
    return res;
}

struct vector rt_vector_arith(struct vector *a, struct vector *b, u_int32_t op) {
    // At this point, the vectors should be of the same type (innerty and size)
    // As well, only vectors of real and float are supported

    struct vector res = vector_new(a->type, a->size);


    if (res.type == VECTOR_TYPE_INT) {
        VECTOR_DATA_INIT_RES(int)
        for (int i = 0; i < res.size; i++) {
            ARITHMETIC_OPS
        }

    } else if (res.type == VECTOR_TYPE_FLOAT) {
        VECTOR_DATA_INIT_RES(float)
        for (int i = 0; i < res.size; i++) {
            ARITHMETIC_OPS
        }
    } else {
        exit(1);
    }

    return res;
}


enum vector_comparison_op_kind {
    LT = 0, GT, LTEQ, GTEQ
};


#define VECTOR_COMPARISON \
switch (op) {             \
VECTOR_OP(LT, <)          \
VECTOR_OP(GT, >)          \
VECTOR_OP(LTEQ, <=)       \
VECTOR_OP(GTEQ, >=)       \
default:            \
exit(1);                  \
}



struct vector rt_vector_comp(struct vector *a, struct vector *b, u_int32_t op) {
    // At this point, the vectors should be of the same type (innerty and size)
    // As well, only vectors of real and float are supported

    struct vector res = vector_new(VECTOR_TYPE_BOOL, a->size);

    if (a->type == VECTOR_TYPE_INT) {
        VECTOR_RES_INIT(char)
        VECTOR_DATA_INIT(int)
        for (int i = 0; i < res.size; i++) {
            VECTOR_COMPARISON
        }

    } else if (b->type == VECTOR_TYPE_FLOAT) {
        VECTOR_RES_INIT(char)
        VECTOR_DATA_INIT(float)
        for (int i = 0; i < res.size; i++) {
            VECTOR_COMPARISON
        }
    } else {
        exit(1);
    }

    return res;
}

enum vector_logical_op_kind {
    AND = 0, OR, XOR, EQ, NEQ
};

#define VECTOR_EQ_OP(enum_name, opposite_op) \
    case enum_name:                          \
        if (a_data[i] opposite_op b_data[i]) \
            return 0;                         \
        break;

#define EQUALITY_OPS \
switch (op) {          \
    VECTOR_EQ_OP(EQ, !=)  \
    VECTOR_EQ_OP(NEQ, ==) \
    default:           \
        exit(1);       \
}

struct vector rt_vector_logical(struct vector *a, struct vector *b, u_int32_t op) {
    // At this point, the vectors should be of the same type (innerty and size)
    // As well, only vectors of real and float are supported

    struct vector res = vector_new(a->type, a->size);

    if (res.type == VECTOR_TYPE_BOOL) {
        VECTOR_DATA_INIT_RES(char)
        for (int i = 0; i < res.size; i++) {
            switch (op) {
                case AND:
                    res_data[i] = a_data[i] && b_data[i];
                    break;
                case OR:
                    res_data[i] = a_data[i] || b_data[i];
                    break;
                case XOR:
                    res_data[i] = a_data[i] ^ b_data[i];
                    break;
                default:
                    exit(1);
            }
        }
    } else {
        exit(1);
    }
    return res;
}

u_int8_t rt_vector_eq(struct vector *a, struct vector *b, u_int32_t op) {

    for (int i = 0; i < a->size; i++) {
        if (a->type == VECTOR_TYPE_INT) {
            VECTOR_DATA_INIT(int)
            EQUALITY_OPS
        } else if (a->type == VECTOR_TYPE_FLOAT) {
            VECTOR_DATA_INIT(float)
            EQUALITY_OPS
        } else if (a->type == VECTOR_TYPE_BOOL || a->type == VECTOR_TYPE_CHAR) {
            VECTOR_DATA_INIT(char)
            EQUALITY_OPS
            exit(1);
        }
    }

    return 1;
}

#define VECTOR_CONCAT \
for (int i = 0; i < a->size; i++) { \
res_data[i] = a_data[i]; \
} \
for (int i = 0; i < b->size; i++) { \
res_data[i + a->size] = b_data[i]; \
}

struct vector rt_vector_concat(struct vector *a, struct vector *b) {

    struct vector res = vector_new(a->type, a->size + b->size);

    if (res.type == VECTOR_TYPE_INT) {
        VECTOR_DATA_INIT_RES(int)
        VECTOR_CONCAT
    } else if (res.type == VECTOR_TYPE_FLOAT) {
        VECTOR_DATA_INIT_RES(float)
        VECTOR_CONCAT
    } else if (res.type == VECTOR_TYPE_BOOL || res.type == VECTOR_TYPE_CHAR) {
        VECTOR_DATA_INIT_RES(char)
        VECTOR_CONCAT
    } else {
        exit(1);
    }

    return res;

}

#define VECTOR_DOTPRODUCT \
for (int i = 0; i < a->size; i++) { \
res += a_data[i] * b_data[i]; \
}

u_int32_t rt_vector_dotproduct_int(struct vector *a, struct vector *b) {
    u_int32_t res = 0;
    VECTOR_DATA_INIT(int)
    VECTOR_DOTPRODUCT
    return res;
}

float rt_vector_dotproduct_real(struct vector *a, struct vector *b) {
    float res = 0;
    VECTOR_DATA_INIT(float)
    VECTOR_DOTPRODUCT
    return res;
}

#define VECTOR_BY \
for (u_int32_t i = 0; i < res.size; i++) { \
res_data[i] = a_data[i*stride];            \
}

struct vector rt_vector_by(struct vector *a, u_int32_t stride) {
    struct vector res = vector_new(a->type, (a->size + stride - 1) / stride);

    if (res.type == VECTOR_TYPE_INT) {
        VECTOR_DATA_INIT_SINGLE(int)
        VECTOR_BY
    } else if (res.type == VECTOR_TYPE_FLOAT) {
        VECTOR_DATA_INIT_SINGLE(float)
        VECTOR_BY
    } else if (res.type == VECTOR_TYPE_BOOL || res.type == VECTOR_TYPE_CHAR) {
        VECTOR_DATA_INIT_SINGLE(char)
        VECTOR_BY
    } else {
        exit(1);
    }

    return res;

}

struct stream_store s = {0, 0, 0, 0, 0, {0}};

// TODO test limits of integer

void rt_print_vector(struct vector *v) {
    printf("[");
    if (v->type == VECTOR_TYPE_INT) {
        int *data = (int *) v->data;
        for (int i = 0; i < v->size; i++) {
            printf("%d", data[i]);
            if (i != v->size - 1)
                printf(" ");
        }
    } else if (v->type == VECTOR_TYPE_FLOAT) {
        float *data = (float *) v->data;
        for (int i = 0; i < v->size; i++) {
            printf("%g", data[i]);
            if (i != v->size - 1)
                printf(" ");
        }
    } else if (v->type == VECTOR_TYPE_CHAR) {
        char *data = (char *) v->data;
        for (int i = 0; i < v->size; i++) {
            printf("%c", data[i]);
            if (i != v->size - 1)
                printf(" ");
        }
    } else if (v->type == VECTOR_TYPE_BOOL) {
        char *data = (char *) v->data;
        for (int i = 0; i < v->size; i++) {
            printf("%c", data[i] ? 'T' : 'F');
            if (i != v->size - 1)
                printf(" ");
        }
    }
    printf("]");
}

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

void queue_char(char c) {
    // check if the buffer is full
    if ((s.back + 1) % 1025 == s.front) {
        ERR_BUFFER_OVERFLOW();
    } else {
        s.buffer[s.back] = c;
        s.back = (s.back + 1) % 1025;
    }
}

char dequeue_char() {
    // check if the buffer is empty
    if (s.front == s.back) {
        ERR_BUFFER_OVERFLOW();
    } else {
        char c = s.buffer[s.front];
        s.front = (s.front + 1) % 1025;
        return c;
    }
}

char dequeue_curr_char() {
    // check if the buffer is empty
    if (s.curr == s.back) {
        ERR_BUFFER_OVERFLOW();
    } else {
        char c = s.buffer[s.curr];
        s.curr = (s.curr + 1) % 1025;
        return c;
    }
}


char consume_char() {
    char c;
    if (s.back + 1 == s.front) ERR_BUFFER_OVERFLOW();
    // check if the buffer is empty
    if (s.front == s.back) {
        // read from the stream
        if (s.eof) { return -1; }
        if (scanf("%c", &c) == EOF) {
            s.eof = 1;
            c = -1;
        }
        queue_char(c);
        s.front = s.back;
    } else {
        // read from the buffer
        c = dequeue_char();
    }

    return c;
}

char is_ws(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}


char* peek_token(char eof_ss) {
    char c;
    s.curr = s.front;
    size_t ws_start = s.front;
    // check if the buffer is empty
    while (1) {
        if (s.curr == s.back) {
            // read from the stream
            if (s.back + 1 == s.front) ERR_BUFFER_OVERFLOW();
            if (scanf("%c", &c) == EOF) {
                if (s.eof) {
                    s.stream_state = eof_ss;
                    return NULL;
                }
                s.eof = 1;
                c = 0;
            }
            queue_char(c);
            s.curr = s.back;
        } else {
            // read from the buffer
            c = dequeue_curr_char();
        }
        size_t len = s.front > s.back ? (1024 - s.front + s.back) : (s.back - s.front);
        if ((is_ws(c) || s.eof) && len > 1 && !is_ws(s.buffer[(s.curr - 2) % 1025])) break;
        if (is_ws(c)) ws_start = s.curr;
    }

    // calculate size of token
    size_t buffer_size = ws_start > s.back ? (1024 - ws_start + s.back + 1) : (s.back - ws_start - 1) + 1;

    // copy the token into a new buffer
    char *token = malloc(buffer_size + 1);
    size_t i = 0;
    while (i != buffer_size) {
        token[i] = s.buffer[ws_start];
        ws_start = (ws_start + 1) % 1025;
        i++;
    }

    token[i-1] = '\0';
    return token;
}

void consume_to_curr() {
    // set front of buffer to current position
    s.front = (s.curr - 1) % 1025;
}

void consume_to_curr_one() {
    s.front = s.curr;
}

void consume_single() {
    // increment front of buffer
    s.front = (s.front + 1) % 1025;
}

char peek_next() {
    char c = ' ';
    // While the current char is whitespace, we have not reached the end of file, or
    while (is_ws(c) && !s.eof && (s.back + 1) % 1025 != s.front) {
        // check if the buffer is empty
        if (s.curr == s.back) {
            // read from the stream
            if (scanf("%c", &c) == EOF) {
                s.eof = 1;
            }
            // put the character back
            s.buffer[s.back] = c;
            s.back = (s.back + 1) % 1025;
        } else {
            // read from the buffer
            c = s.buffer[s.curr];
        }
        s.curr = (s.curr + 1) % 1025;
    }

    if (s.eof) {
        return EOF;
    } else {
        return c;
    }
}

char peek_char() {
    // reset peek to beginning
    s.curr = s.front;
    return peek_next();

}

char rt_scan_char() {
    // TODO fix one missing after final
    char temp = consume_char();
    s.stream_state = 0;
    return temp;
}

char rt_scan_bool() {
    char c = peek_char();
    if (c == 'T') {
        consume_to_curr_one();
        s.stream_state = 0;
        return 1;
    } else if (c == 'F') {
        consume_to_curr_one();
        s.stream_state = 0;
        return 0;
    } else if (c == EOF) {
        consume_single();
        s.stream_state = 1;
        return 0;
    } else {
        consume_single();
        s.stream_state = 2;
        return 0;
    }
}


float scan_real_err(char val) {
    s.stream_state = val;
    return 0.0f;
}

int scan_int_err(char val) {
    s.stream_state = val;
    return 0;
}

int rt_scan_int() {

    char *token = peek_token(2);
    if (token == NULL) return scan_int_err(2);

    // convert the token to an int
    char *end;
    int num = strtol(token, &end, 10);
    if (end == token) return scan_int_err(1);
    if (!is_ws(*end) && *end != '\0') return scan_int_err(1);
    s.stream_state = 0;

    free(token);
    consume_to_curr();

    return num;
}

float rt_scan_real() {
    char *token = peek_token(2);
    if (token == NULL) return scan_real_err(2);

    char *end;
    float num = strtof(token, &end);
    if (end == token) return scan_real_err(1);
    if (!is_ws(*end) && *end != '\0') return scan_real_err(1);

    s.stream_state = 0;

    free(token);
    consume_to_curr();
    return num;
}




// sample main function for testing

int main() {


    int i;
    float d;
    char c = 0;
    char b;
    char sample;
//
//    while (1) {
    i = rt_scan_int();
    printf("i: %d, state: %d\n", i, s.stream_state);
    sample = rt_scan_char();
    printf("c: %c, state: %d\n", sample, s.stream_state);

    c = rt_scan_char();
    printf("c: %c, state: %d\n", c, s.stream_state);
    sample = rt_scan_char();
    printf("c: %c, state: %d\n", sample, s.stream_state);

    b = rt_scan_bool();
    printf("b: %d, state: %d\n", b, s.stream_state);
    sample = rt_scan_char();
    printf("c: %c, state: %d\n", sample, s.stream_state);

    d = rt_scan_real();
    printf("val: %g, state: %d\n", d, s.stream_state);
    rt_print_int(i);
    rt_print_char(c);
    rt_print_bool(b);
    rt_print_real(d);
//
//        if (s.stream_state != 0) break;
//    }
//
    return 0;
}
