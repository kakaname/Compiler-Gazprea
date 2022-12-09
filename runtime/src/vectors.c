//
// Created by rajan on 12/01/22.
//

#include "runtime.h"

struct vector *rt_vector_new(int64_t type, u_int64_t size) {
    struct vector *v = malloc(sizeof(struct vector));
    v->size = size;
    v->idx = rt_get_seq_idx(size);
    v->type = type;
    v->data = rt_get_data_alloc_for_vec(size, type, NULL);
  
    return v;
}

struct vector *rt_vector_empty_copy(int64_t type, struct vector *v) {
    struct vector *res = rt_vector_new(type, v->size);
    return res;
}

char rt_vector_out_of_bounds(struct vector *v, u_int64_t idx) {
    return (idx > v->size || idx == 0) ? 1 : 0;
}

struct vector *rt_vector_not(struct vector *a) {
    struct vector *res = rt_vector_new(a->type, a->size);
    VECTOR_DATA_INIT_SINGLE(char)
    for (int64_t i = 0; i < a->size; i++) {
        res_data[i] = !a_data[i];
    }
    return res;
}

struct vector *rt_vector_sub(struct vector *a) {
    struct vector *res = rt_vector_new(a->type, a->size);
    if (a->type == VECTOR_TYPE_INT) {
        VECTOR_DATA_INIT_SINGLE(int64_t)
        for (int64_t i = 0; i < a->size; i++) {
            res_data[i] = -a_data[i];
        }
    } else if (a->type == VECTOR_TYPE_FLOAT) {
        VECTOR_DATA_INIT_SINGLE(float)
        for (int64_t i = 0; i < a->size; i++) {
            res_data[i] = -a_data[i];
        }
    } else {
        exit(1);
    }
    return res;
}

struct vector *rt_vector_arith(struct vector *a, struct vector *b, u_int64_t op) {
    // At this point, the vectors should be of the same type (innerty and size)
    // As well, only vectors of real and float are supported

    struct vector *res = rt_vector_new(a->type, a->size);

    if (res->type == VECTOR_TYPE_INT) {
        VECTOR_DATA_INIT_RES(int64_t)
        for (int64_t i = 0; i < res->size; i++) {
            switch (op) {
                ARITHMETIC_OPS
                case EXP:
                    res_data[i] = rt_ipow(a_data[i], b_data[i]);
                    break;
                case MOD:
                    res_data[i] = a_data[i] % b_data[i];
                    break;
            }
        }

    } else if (res->type == VECTOR_TYPE_FLOAT) {
        VECTOR_DATA_INIT_RES(float)
        for (int64_t i = 0; i < res->size; i++) {
            switch (op) {
                ARITHMETIC_OPS
                case EXP:
                    res_data[i] = powf(a_data[i], b_data[i]);
                    break;
                case MOD:
                    res_data[i] = remainderf(a_data[i], b_data[i]);
                    break;
            }
        }
    } else {
        exit(1);
    }

    return res;
}


struct vector *rt_vector_comp(struct vector *a, struct vector *b, u_int64_t op) {
    // At this point, the vectors should be of the same type (innerty and size)
    // As well, only vectors of real and float are supported

    struct vector *res = rt_vector_new(VECTOR_TYPE_BOOL, a->size);

    if (a->type == VECTOR_TYPE_INT) {
        VECTOR_RES_INIT(char)
        VECTOR_DATA_INIT(int64_t)
        for (int64_t i = 0; i < res->size; i++) {
            VECTOR_COMPARISON
        }

    } else if (b->type == VECTOR_TYPE_FLOAT) {
        VECTOR_RES_INIT(char)
        VECTOR_DATA_INIT(float)
        for (int64_t i = 0; i < res->size; i++) {
            VECTOR_COMPARISON
        }
    } else {
        exit(1);
    }

    return res;
}

struct vector *rt_vector_logical(struct vector *a, struct vector *b, u_int64_t op) {
    // At this point, the vectors should be of the same type (innerty and size)
    // As well, only vectors of real and float are supported

    struct vector *res = rt_vector_new(a->type, a->size);

    if (res->type == VECTOR_TYPE_BOOL) {
        VECTOR_DATA_INIT_RES(char)
        for (int64_t i = 0; i < res->size; i++) {
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

u_int8_t rt_vector_eq(struct vector *a, struct vector *b, u_int64_t op) {

    for (int64_t i = 0; i < a->size; i++) {
        if (a->type == VECTOR_TYPE_INT) {
            VECTOR_DATA_INIT(int64_t)
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

struct vector *rt_vector_concat(struct vector *a, struct vector *b) {

    struct vector *res = rt_vector_new(a->type, a->size + b->size);

    if (res->type == VECTOR_TYPE_INT) {
        VECTOR_DATA_INIT_RES(int64_t)
        VECTOR_CONCAT
    } else if (res->type == VECTOR_TYPE_FLOAT) {
        VECTOR_DATA_INIT_RES(float)
        VECTOR_CONCAT
    } else if (res->type == VECTOR_TYPE_BOOL || res->type == VECTOR_TYPE_CHAR) {
        VECTOR_DATA_INIT_RES(char)
        VECTOR_CONCAT
    } else {
        exit(1);
    }

    return res;

}

int64_t rt_vector_dotproduct_int(struct vector *a, struct vector *b) {
    int64_t res = 0;
    VECTOR_DATA_INIT(int64_t)
    VECTOR_DOTPRODUCT
    return res;
}

float rt_vector_dotproduct_real(struct vector *a, struct vector *b) {
    float res = 0;
    VECTOR_DATA_INIT(float)
    VECTOR_DOTPRODUCT
    return res;
}

struct vector *rt_vector_by(struct vector *a, u_int64_t stride) {
    struct vector *res = rt_vector_new(a->type, (a->size + stride - 1) / stride);

    if (res->type == VECTOR_TYPE_INT) {
        VECTOR_DATA_INIT_SINGLE(int64_t)
        VECTOR_BY
    } else if (res->type == VECTOR_TYPE_FLOAT) {
        VECTOR_DATA_INIT_SINGLE(float)
        VECTOR_BY
    } else if (res->type == VECTOR_TYPE_BOOL || res->type == VECTOR_TYPE_CHAR) {
        VECTOR_DATA_INIT_SINGLE(char)
        VECTOR_BY
    } else {
        exit(1);
    }

    return res;

}

VECTOR_ACCESS(char)

VECTOR_ACCESS(int64_t)

VECTOR_ACCESS(float)



struct vector *rt_vector_create_deep_copy(struct vector *v) {
    struct vector *newV = malloc(sizeof(struct vector));
    newV->size = v->size;
    newV->idx = rt_get_seq_idx(v->size);
    newV->type = v->type;
    newV->data = rt_get_data_alloc_for_vec(newV->size, newV->type, NULL);

    switch (v->type) {
        case VECTOR_TYPE_BOOL:
        case VECTOR_TYPE_CHAR:
            for (int64_t i = 0; i < v->size; i++) {
                ((char *) newV->data)[i] = rt_vector_access_char(v, i+1, 0);
            }
            break;
        case VECTOR_TYPE_INT:
            for (int64_t i = 0; i < v->size; i++) {
                ((int64_t *) newV->data)[i] = rt_vector_access_int64_t(v, i+1, 0);
            }
            break;
        case VECTOR_TYPE_FLOAT:
            for (int64_t i = 0; i < v->size; i++) {
                ((float *) newV->data)[i] = rt_vector_access_float(v, i+1, 0);
            }
            break;
    }
    return newV;
}

VECTOR_SET(char)

VECTOR_SET(int64_t)

VECTOR_SET(float)


struct vector *rt_vector_view_scalar(struct vector *v, u_int64_t idx) {
    struct vector *newV = malloc(sizeof(struct vector));

    newV->size = 1;
    newV->idx = malloc(sizeof(u_int64_t));
    newV->idx[0] = idx;
    newV->type = v->type;
    newV->data = v->data;
    return newV;
}

struct vector *rt_vector_view_vector(struct vector *v, struct vector *idx) {
    struct vector *newV = malloc(sizeof(struct vector));

    // TODO confirm this works with the test case

    newV->size = idx->size;
    newV->idx = idx->data;
    newV->type = v->type;
    newV->data = v->data;
    return newV;
}

int64_t rt_ipow(int64_t base, int64_t exp) {
    // taken from https://stackoverflow.com/questions/101439/the-most-efficient-way-to-implement-an-integer-based-power-function-powint-int
    int64_t result = 1;
    for (;;)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }

    return result;
}

