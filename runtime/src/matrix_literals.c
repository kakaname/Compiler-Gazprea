//
// Created by dhanrajbir on 09/12/22.
//

#include "runtime.h"

static struct vector **current_vecs = NULL;
static int64_t literal_size = 0;
static enum vector_type target_type = VECTOR_TYPE_INT;


void rt_init_matrix_literal__(int64_t size, int64_t type) {
    current_vecs = malloc(sizeof(struct vector*) * size);
    literal_size = size;
    target_type = type;
}

void rt_add_vector_to_matrix_literal__(struct vector *vec, int64_t idx) {
    current_vecs[idx] = vec;
}

struct matrix *rt_get_built_matrix_literal__() {
    int64_t max_size = 0;
    struct matrix *new_mat = malloc(sizeof (struct matrix));
    for (int64_t i = 0; i < literal_size; i++)
        max_size = (max_size < current_vecs[i]->size) ? current_vecs[i]->size : max_size;

    new_mat->rows = literal_size;
    new_mat->cols = max_size;
    new_mat->idx = rt_get_seq_idx(literal_size);
    new_mat->type = target_type;
    new_mat->data = malloc(sizeof (struct vector*) * literal_size);
    for (int64_t i = 0; i < literal_size; i++) {
        new_mat->data[i] = rt_get_casted_vector(current_vecs[i], max_size, target_type);
        free(current_vecs[i]->data);
        free(current_vecs[i]->idx);
    }
    free(current_vecs);
    return new_mat;
}