//
// Created by rajan on 12/01/22.
//

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int64_t rt_ipow(int64_t base, int64_t exp);

#define VECTOR_OP(enum_name, op) \
    case enum_name:                  \
        res_data[i] = a_data[i] op b_data[i]; \
        break;
#define ARITHMETIC_OPS \
    VECTOR_OP(MUL, *)  \
    VECTOR_OP(DIV, /)  \
    VECTOR_OP(ADD, +)  \
    VECTOR_OP(SUB, -)  \
    default:           \
        exit(1);

#define VECTOR_A_INIT(type) \
    type *a_data = (type *) a->data;
#define VECTOR_RES_INIT(type) \
    type *res_data = (type *) res->data;
#define VECTOR_DATA_INIT_SINGLE(type) \
    VECTOR_A_INIT(type) \
    VECTOR_RES_INIT(type)
#define VECTOR_DATA_INIT(type) \
    VECTOR_A_INIT(type)        \
    type *b_data = (type *) b->data;
#define VECTOR_DATA_INIT_RES(type) \
    VECTOR_DATA_INIT(type)         \
    type *res_data = (type *) res->data;
#define VECTOR_COMPARISON \
switch (op) {             \
VECTOR_OP(LT, <)          \
VECTOR_OP(GT, >)          \
VECTOR_OP(LTEQ, <=)       \
VECTOR_OP(GTEQ, >=)       \
default:            \
exit(1);                  \
}
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
#define VECTOR_CONCAT \
for (int64_t i = 0; i < a->size; i++) { \
res_data[i] = a_data[i]; \
} \
for (int64_t i = 0; i < b->size; i++) { \
res_data[i + a->size] = b_data[i]; \
}
#define VECTOR_DOTPRODUCT \
for (int64_t i = 0; i < a->size; i++) { \
res += a_data[i] * b_data[i]; \
}
#define VECTOR_BY \
for (u_int64_t i = 0; i < res->size; i++) { \
res_data[i] = a_data[i*stride];            \
}
#define VECTOR_ACCESS(type) \
type rt_vector_access_##type(struct vector *v, u_int64_t idx, u_int64_t unchecked) { \
    if (idx >= v->size) {   \
        if (unchecked) {  \
            return 0; \
        } else {            \
            exit(1); \
        } \
    }                       \
    u_int64_t real_idx = idx;                                                        \
    if (v->idx != 0) { \
        real_idx = v->idx[idx];\
    }                       \
    return ((type *) v->data)[real_idx];                        \
}
#define VECTOR_SET(type) \
void rt_vector_set_##type(struct vector *v, u_int64_t idx, type val, u_int64_t unchecked) { \
    if (idx >= v->size) {\
        if (unchecked) {  \
            return; \
        } else {         \
            exit(1); \
        } \
    }                    \
    u_int64_t real_idx = idx; \
    if (v->idx != 0) { \
        real_idx = v->idx[idx];\
    }                    \
                         \
    ((type *) v->data)[real_idx] = val;                        \
}
#define MATRIX_ACCESS(type) \
type rt_matrix_access_##type(struct matrix *m, u_int64_t row, u_int64_t col, u_int64_t unchecked) { \
    if (row >= m->rows || col >= m->cols) { \
        if (unchecked) { \
            return 0; \
        } else { \
            exit(1); \
        } \
    }                       \
                            \
    u_int64_t row_idx = row;\
    if (m->idx != 0) { \
        row_idx = m->idx[row]; \
    } \
    struct vector *v = m->data[row]; \
    return rt_vector_access_##type(v, col, unchecked);                    \
}
#define MATRIX_SET(type) \
void rt_matrix_set_##type(struct matrix *m, u_int64_t row, u_int64_t col, type val, u_int64_t unchecked) { \
    if (row >= m->rows || col >= m->cols) { \
        if (unchecked) { \
            return; \
        } else { \
            exit(1); \
        } \
    }                    \
 \
    u_int64_t row_idx = row;\
    if (m->idx != 0) { \
        row_idx = m->idx[row]; \
    } \
    struct vector *v = m->data[row]; \
    rt_vector_set_##type(v, col, val, unchecked);                    \
}
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

enum vector_arithmeic_op_kind {
    EXP = 0,
    MUL,
    DIV,
    MOD,
    ADD,
    SUB
};
enum vector_comparison_op_kind {
    LT = 0, GT, LTEQ, GTEQ
};
enum vector_logical_op_kind {
    AND = 0, OR, XOR, EQ, NEQ
};

struct vector {
    u_int64_t size;
    u_int64_t *idx;
    u_int64_t type;
    void *data;
};

struct matrix {

    u_int64_t rows;
    u_int64_t cols;
    u_int64_t *idx;
    u_int64_t type;
    struct vector **data;
};


struct vector *rt_vector_new(enum vector_type type, u_int64_t size);
struct vector *rt_vector_not(struct vector *v);
struct vector *rt_vector_sub(struct vector *v);
struct vector *rt_vector_arith(struct vector *a, struct vector *b, u_int64_t op);
struct vector *rt_vector_comp(struct vector *a, struct vector *b, u_int64_t op);
struct vector *rt_vector_logical(struct vector *a, struct vector *b, u_int64_t op);
u_int8_t rt_vector_eq(struct vector *a, struct vector *b, u_int64_t op);
char rt_vector_access_char(struct vector *v, u_int64_t idx, u_int64_t unchecked);
int64_t rt_vector_access_int64_t(struct vector *v, u_int64_t idx, u_int64_t unchecked);
float rt_vector_access_float(struct vector *v, u_int64_t idx, u_int64_t unchecked);
void rt_vector_set_char(struct vector *v, u_int64_t idx, char val, u_int64_t unchecked);
void rt_vector_set_int64_t(struct vector *v, u_int64_t idx, int64_t val, u_int64_t unchecked);
void rt_vector_set_float(struct vector *v, u_int64_t idx, float val, u_int64_t unchecked);
struct vector *rt_vector_create_deep_copy(struct vector *v);
struct vector *rt_vector_view_scalar(struct vector *v, u_int64_t idx);
struct vector *rt_vector_view_vector(struct vector *v, struct vector *idx);

int64_t *rt_get_seq_idx(int64_t size);
void* rt_get_data_alloc_for_vec(int64_t size, enum vector_type type, void *default_val);
struct vector *rt_vector_empty_copy(enum vector_type type, struct vector *v);

struct vector *rt_get_casted_vector(struct vector *base, int64_t size, int64_t target_type);
