//
// Created by dhanrajbir on 07/12/22.
//

#include "runtime.h"

#define GET_CASTED_VEC_LOOP_INNER(BASE, NEW_ALLOC, TARGET_SIZE, TARGET_TYPE, CASTED_BASE) \
    for (int64_t i = 0; i < TARGET_SIZE; ++i) { \
        if(i < BASE->size) {    \
            cast_scalar_value(&CASTED_BASE[BASE->idx[i]], NEW_ALLOC+i, BASE->type, TARGET_TYPE); \
            continue; \
        } \
        cast_scalar_value(NULL, NEW_ALLOC+i, BASE->type, TARGET_TYPE); \
    }                                                                                \

#define GET_CASTED_VEC_LOOP(BASE, NEW_ALLOC, TARGET_SIZE, TARGET_TYPE, CASTED_BASE) \
        switch (TARGET_TYPE) {\
            case VECTOR_TYPE_BOOL: {\
                unsigned char *casted_alloc = (unsigned char *) NEW_ALLOC;\
                GET_CASTED_VEC_LOOP_INNER(BASE, casted_alloc, TARGET_SIZE, TARGET_TYPE, CASTED_BASE);\
                break;                                                              \
            }\
            case VECTOR_TYPE_INT: {                                                 \
                int64_t *casted_alloc = (int64_t *) NEW_ALLOC;\
                GET_CASTED_VEC_LOOP_INNER(BASE, casted_alloc, TARGET_SIZE, TARGET_TYPE, CASTED_BASE);\
                break;                                                              \
            }\
            case VECTOR_TYPE_FLOAT: {\
                float *casted_alloc = (float *) NEW_ALLOC;                          \
                GET_CASTED_VEC_LOOP_INNER(BASE, casted_alloc, TARGET_SIZE, TARGET_TYPE, CASTED_BASE);\
                break;\
            }\
            case VECTOR_TYPE_CHAR: {\
                unsigned char *casted_alloc = (unsigned char *) NEW_ALLOC;\
                GET_CASTED_VEC_LOOP_INNER(BASE, casted_alloc, TARGET_SIZE, TARGET_TYPE, CASTED_BASE);\
                break;                                                              \
            }                                                                           \
        }\


static int64_t *get_seq_idx(int64_t size) {
    int64_t *idx = malloc(size * sizeof(int64_t));
    for (int64_t i = 0; i < size; i++)
        idx[i] = i;
    return idx;
}

static void cast_scalar_value(void *src, void *dest,
                       enum vector_type src_ty, enum vector_type dest_ty) {
    switch (dest_ty) {
        case VECTOR_TYPE_BOOL:
            switch (src_ty) {
                case VECTOR_TYPE_BOOL:
                    *((unsigned char* ) dest) = (src) ? *((unsigned char *) src) : 0;
                    return;
                case VECTOR_TYPE_FLOAT:
                    *((unsigned char* ) dest) = (src) ? (*((float *) src) != 0) : 0;
                    return;
                case VECTOR_TYPE_INT:
                    *((unsigned char* ) dest) = (src) ? (*((int64_t *) src) != 0) : 0;
                    return;
                case VECTOR_TYPE_CHAR:
                    *((unsigned char* ) dest) = (src) ? (*((char *) src) != 0) : 0;
                    return;
            }
        case VECTOR_TYPE_INT:
            switch (src_ty) {
                case VECTOR_TYPE_BOOL:
                    *((int64_t *) dest) = (src) ? *((unsigned char*) src) == 1 : 0;
                    return;
                case VECTOR_TYPE_INT:
                    *((int64_t *) dest) = (src) ? *((int64_t *) src) : 0;
                    return;
                case VECTOR_TYPE_FLOAT:
                    *((int64_t *) dest) = (src) ? *((float *) src) : 0;
                    return;
                case VECTOR_TYPE_CHAR:
                    printf("Casting from char to integer\n");
                    *((int64_t *) dest) = (src) ?  *((unsigned char *) src) : 0;
                    return;
            }
        case VECTOR_TYPE_FLOAT:
            switch (src_ty) {
                case VECTOR_TYPE_BOOL:
                    *((float *) dest) = (src) ?  *((unsigned char *) src) == 1 : 0;
                    return;
                case VECTOR_TYPE_CHAR:
                    *((float *) dest) = (src) ? *((unsigned char *) src) : 0;
                    return;
                case VECTOR_TYPE_INT:
                    *((float *) dest) = (src) ? *((int64_t *) src) : 0;
                    return;
                case VECTOR_TYPE_FLOAT:
                    *((float *) dest) = (src) ? *((float *) src) : 0;
                    return;
            }
        case VECTOR_TYPE_CHAR:
            switch (src_ty) {
                case VECTOR_TYPE_BOOL:
                    *((unsigned char *) dest) = (src) ? *((unsigned char *) src) == 1 : 0;
                    return;
                case VECTOR_TYPE_CHAR:
                    *((unsigned char *) dest) = (src) ? *((unsigned char *) src) : 0;
                    return;
                case VECTOR_TYPE_INT:
                    *((unsigned char *) dest) = (src) ? *((int64_t *) src) % 256 : 0;
                    return;
                case VECTOR_TYPE_FLOAT:
                    *((unsigned char *) dest) = (src) ? *((int64_t *) src) : 0;
                    return;
            }
    }
}

static void* get_data_alloc_for_vec(int64_t size, enum vector_type type, void *default_val) {
    switch (type) {
        case VECTOR_TYPE_BOOL:
        case VECTOR_TYPE_CHAR: {
            unsigned char set_val = (default_val) ? *((unsigned char *) default_val) : 0;
            unsigned char *new_alloc = malloc(size * sizeof(unsigned char));
            for (int64_t i = 0; i < size; i++)
                new_alloc[i] = set_val;
            return new_alloc;
        }
        case VECTOR_TYPE_INT: {
            int64_t set_val = (default_val) ? *((int64_t*) default_val) : 0;
            int64_t *new_alloc = malloc(size * sizeof(int64_t));
            for (int64_t i = 0; i < size; i++)
                new_alloc[i] = set_val;
            return new_alloc;
        }
        case VECTOR_TYPE_FLOAT: {
            float set_val = (default_val) ? *((float*) default_val) : 0;
            float *new_alloc = malloc(size * sizeof(float));
            for (int64_t i = 0; i < size; i++)
                new_alloc[i] = set_val;
            return new_alloc;
        }
    }
}

struct vector *rt_get_same_vector_as(struct vector *target, void *data) {
    struct vector *new_vec = malloc(sizeof(struct vector));
    new_vec->type = target->type;
    new_vec->size = target->size;
    new_vec->data = get_data_alloc_for_vec(target->size, target->type, data);
    new_vec->idx = get_seq_idx(target->size);
    return new_vec;
}

struct matrix *rt_get_same_matrix_as(struct matrix *target, void *data) {
    struct matrix *new_mat = malloc(sizeof(struct matrix));
    new_mat->type = target->type;
    new_mat->rows = target->rows;
    new_mat->cols = target->cols;
    new_mat->data = malloc(target->rows * sizeof(struct vector*));
    new_mat->idx = get_seq_idx(target->rows);
    for (int64_t i = 0; i < target->rows; i++)
        new_mat->data[i] = rt_get_same_vector_as(target->data[target->idx[i]], data);
    return new_mat;
}

struct vector *rt_get_casted_vector(struct vector *base,
        int64_t size, int64_t target_type) {
    int64_t target_size = (size > -1) ? size : base->size;
    struct vector *new_vec = malloc(sizeof(struct vector));
    new_vec->type = base->type;
    new_vec->size = target_size;
    new_vec->idx = get_seq_idx(target_size);

    void *new_alloc = get_data_alloc_for_vec(target_size, target_type, NULL);
    switch (base->type) {
        case VECTOR_TYPE_BOOL: {
            unsigned char *casted_base = (unsigned char *) base->data;
            GET_CASTED_VEC_LOOP(base, new_alloc, target_size, target_type, casted_base);
            break;
        }
        case VECTOR_TYPE_INT: {
            int64_t *casted_base = (int64_t *) base->data;
            GET_CASTED_VEC_LOOP(base, new_alloc, target_size, target_type, casted_base);
            break;
        }
        case VECTOR_TYPE_FLOAT: {
            float *casted_base = (float *) base->data;
            GET_CASTED_VEC_LOOP(base, new_alloc, target_size, target_type, casted_base);
            break;
        }
        case VECTOR_TYPE_CHAR: {
            unsigned char *casted_base = (unsigned char *) base->data;
            GET_CASTED_VEC_LOOP(base, new_alloc, target_size, target_type, casted_base);
            break;
        }
        default:
            {}
    }
    new_vec->data = new_alloc;
    return new_vec;
}

struct matrix *rt_get_casted_matrix(struct matrix* base, int64_t rows,
        int64_t cols, enum vector_type target_type) {
    int64_t target_rows = (rows > -1) ? rows : base->rows;
    int64_t target_cols = (cols > -1) ? cols : base->cols;

    struct matrix *new_mat = malloc(sizeof (struct matrix));
    new_mat->rows = target_rows;
    new_mat->cols = target_cols;
    new_mat->type = target_type;
    new_mat->idx = get_seq_idx(target_rows);

    struct vector **new_alloc = malloc(sizeof(struct vector *) * target_rows);

    for (int64_t i = 0; i < target_rows; i++) {
        if (i < base->rows) {
            new_alloc[i] = rt_get_casted_vector(base->data[i], target_cols, target_type);
            continue;
        }
        struct vector *row = malloc(sizeof (struct vector));
        row->type = target_type;
        row->size = target_cols;
        row->idx = get_seq_idx(target_cols);
        row->data = get_data_alloc_for_vec(target_cols, target_type, NULL);
        new_alloc[i] = row;
    }
    return new_mat;
}

struct vector *rt_get_vector_with_value(int64_t size, enum vector_type type, void *value) {
    struct vector *new_vec = malloc(sizeof (struct vector));
    new_vec->size = size;
    new_vec->idx = get_seq_idx(size);
    new_vec->data = get_data_alloc_for_vec(size, type, value);
    new_vec->type = type;
    return new_vec;
}

struct matrix *rt_get_matrix_with_value(int64_t rows, int64_t cols, enum vector_type type, void *value) {
    struct matrix *new_mat = malloc(sizeof (struct matrix));
    new_mat->rows = rows;
    new_mat->cols = cols;
    new_mat->idx = get_seq_idx(rows);
    new_mat->type = type;
    struct vector **data = malloc(sizeof (struct vector*) * rows);

    for (int64_t i = 0; i < new_mat->rows; i++)
        data[i] = rt_get_vector_with_value(cols, type, value);
    return new_mat;
}

// TODO: Implement this after the interval struct is known.
struct vector *rt_get_vector_from_interval() {return NULL;}