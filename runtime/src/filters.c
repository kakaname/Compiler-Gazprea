//
// Created by dhanrajbir on 08/12/22.
//

#include "runtime.h"

static int64_t num_of_filters = 0;

static char should_add_to_residual = 0;

static struct vector **current_filter_vecs = NULL;


static void assign_to_idx(void *data_alloc, void *data, int64_t idx, enum vector_type type) {
    switch (type) {
        case VECTOR_TYPE_CHAR:
        case VECTOR_TYPE_BOOL:
            *(((unsigned char *) data_alloc) + idx) = *((unsigned char *) data);
        case VECTOR_TYPE_INT:
//            printf("Assigning to idx=%ld val=%ld\n", idx, *((int64_t *) data));
            *(((int64_t *) data_alloc) + idx) = *((int64_t *) data);
        case VECTOR_TYPE_FLOAT:
            *(((float *) data_alloc) + idx) = *((float *) data);
    }
}


void rt_init_filter_expr(struct vector *domain, int64_t number_of_exprs) {
    num_of_filters = number_of_exprs + 1;
    current_filter_vecs = malloc(sizeof(struct vector*) * num_of_filters);
    //printf("Current filters %p\n", current_filters);
    //printf("Init filter with %ld vectors\n", num_of_filters);
    for (int64_t i = 0; i < num_of_filters; i++) {
        struct vector* new_vec = malloc(sizeof (struct vector));
        //printf("New Vec at idx %ld = %p\n", i, new_vec);
        new_vec->type = domain->type;
        new_vec->size = 0;
        new_vec->data = rt_get_data_alloc_for_vec(domain->size, domain->type, NULL);
        current_filter_vecs[i] = new_vec;
    }
    should_add_to_residual = 1;
}

void rt_update_filter_at_pos(int64_t idx, char should_update, void *data) {
    printf("Updating filter at i=%ld should_update=%b\n", idx, !!should_update);
    if (!should_update)
        return;


    struct vector *vec_at_idx = current_filter_vecs[idx];
    int64_t current_size = vec_at_idx->size;
    assign_to_idx(vec_at_idx->data, data, current_size, vec_at_idx->type);
    vec_at_idx->size++;

    printf("Index is i=%ld\n", idx);
    printf("After update at i=%ld\n", idx);
//    printf("Index is i=%ld\n", idx);
    for (int i = 0; i < vec_at_idx->size; i++)
        printf("%f\n", *((float *) vec_at_idx->data + i));

    should_add_to_residual = 0;
}


void rt_filter_end_iter(void *current_value) {
    //printf("Current last vector is at idx %ld\n", num_of_filters-1);
    //printf("Current last vector is at %p\n", residual);

    if (!should_add_to_residual) {
        should_add_to_residual = 1;
        return;
    }
    struct vector *residual = current_filter_vecs[num_of_filters-1];
    assign_to_idx(residual->data, current_value, residual->size, residual->type);
    residual->size++;
}


void rt_write_val_from_vec_to(struct vector* vec, int64_t idx, void *loc) {
    switch (vec->type) {
        case VECTOR_TYPE_BOOL:
//            printf("Writing %d\n", (*((unsigned char *) vec->data + vec->idx[idx]) == 1));
            *((unsigned char *) loc) = (*((unsigned char *) vec->data + vec->idx[idx]) == 1);
            return;
        case VECTOR_TYPE_CHAR:
//            printf("Writing %d\n", *((unsigned char *) vec->data + vec->idx[idx]));
            *((unsigned char *) loc) = *((unsigned char *) vec->data + vec->idx[idx]);
            return;
        case VECTOR_TYPE_INT:
//            printf("Writing %ld\n", *((int64_t *) vec->data + vec->idx[idx]));
            *((int64_t *) loc) = *((int64_t *) vec->data + vec->idx[idx]);
            return;
        case VECTOR_TYPE_FLOAT:
//            printf("Writing %f\n", *((float *) vec->data + vec->idx[idx]));
            *((float *) loc) = *((float *) vec->data + vec->idx[idx]);
            return;
    }
}

struct vector *rt_get_completed_filter_at(int64_t idx) {
    struct vector *to_ret = current_filter_vecs[idx];
    to_ret->idx = rt_get_seq_idx(to_ret->size);
//    printf("Returning vector at idx %ld\n", idx);
//    printf("Vector size %ld\n", to_ret->size);
//    for (int i = 0; i < to_ret->size; i++)
//        printf("%ld, %ld\n", to_ret->idx[i], *((int64_t *) to_ret->data + i));
    return to_ret;
}

void rt_shutdown_filter_expr_builder() {
    free(current_filter_vecs);
    should_add_to_residual = 0;
    num_of_filters = 0;
}