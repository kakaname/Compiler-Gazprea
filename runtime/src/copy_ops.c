//
// Created by dhanrajbir on 08/12/22.
//

#include "runtime.h"

struct vector *rt_get_vec_copy__(struct vector *to_copy) {
    struct vector *copy = malloc(sizeof (struct vector));
    copy->size = to_copy->size;
    copy->type = to_copy->type;
    copy->idx = rt_get_seq_idx(to_copy->size);
    switch (to_copy->type) {
        case VECTOR_TYPE_CHAR:
        case VECTOR_TYPE_BOOL: {
            unsigned char *data = malloc(sizeof (unsigned char ) * copy->size);
            unsigned char *buf_to_copy = (unsigned char *) to_copy->data;
            for (int64_t i = 0; i < to_copy->size; i++)
                data[i] = buf_to_copy[to_copy->idx[i]];
            copy->data = data;
            return copy;
        }
        case VECTOR_TYPE_FLOAT: {
            float *data = malloc(sizeof (float ) * copy->size);
            float *buf_to_copy = (float *) to_copy->data;
            for (int64_t i = 0; i < to_copy->size; i++)
                data[i] = buf_to_copy[to_copy->idx[i]];
            copy->data = data;
            return copy;
        }
        case VECTOR_TYPE_INT: {
            int64_t *data = malloc(sizeof (int64_t ) * copy->size);
            int64_t *buf_to_copy = (int64_t *) to_copy->data;
            for (int64_t i = 0; i < to_copy->size; i++)
                data[i] = buf_to_copy[to_copy->idx[i]];
            copy->data = data;
            return copy;
        }
    }
}

struct matrix *rt_get_matrix_copy__(struct matrix *to_copy) {
    struct matrix *copy = malloc(sizeof (struct matrix));
    copy->rows = to_copy->rows;
    copy->cols = to_copy->cols;
    copy->type = to_copy->type;
    copy->idx = rt_get_seq_idx(copy->rows);
    copy->data = malloc(sizeof(struct matrix*) * copy->rows);
    for (int64_t i = 0; i < copy->rows; i++)
        copy->data[i] = rt_get_vec_copy__(to_copy->data[i]);
    return copy;
}