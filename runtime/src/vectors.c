//
// Created by rajan on 12/01/22.
//

#include "runtime.h"

struct vector *rt_vector_new(enum vector_type type, u_int64_t size) {
    struct vector *v = malloc(sizeof(struct vector));
    v->size = size;
    v->idx = 0;
    v->type = type;
    switch (type) {
        case VECTOR_TYPE_BOOL:
        case VECTOR_TYPE_CHAR:
            // NOTE: booleans are stored as bytes
            v->data = malloc(sizeof(char) * size);
            break;
        case VECTOR_TYPE_INT:
            v->data = malloc(sizeof(int64_t) * size);
            break;
        case VECTOR_TYPE_FLOAT:
            v->data = malloc(sizeof(float) * size);
            break;
        default:
            exit(1);
    }
    return v;
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

    printf("Vector A length: %ld, type: %lul\n", a->size, a->type);
    printf("Vector B length: %ld, type: %lul\n", b->size, b->type);

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
    newV->idx = 0;
    newV->type = v->type;

    if (v->idx == 0) {
        switch (v->type) {
            case VECTOR_TYPE_BOOL:
            case VECTOR_TYPE_CHAR:
                newV->data = malloc(sizeof(char) * v->size);
                memcpy(newV->data, v->data, sizeof(char) * v->size);
                break;
            case VECTOR_TYPE_INT:
                newV->data = malloc(sizeof(int64_t) * v->size);
                memcpy(newV->data, v->data, sizeof(int64_t) * v->size);
                break;
            case VECTOR_TYPE_FLOAT:
                newV->data = malloc(sizeof(float) * v->size);
                memcpy(newV->data, v->data, sizeof(float) * v->size);
                break;
        }
    } else {
        switch (v->type) {
            case VECTOR_TYPE_BOOL:
            case VECTOR_TYPE_CHAR:
                newV->data = malloc(sizeof(char) * v->size);
                for (int64_t i = 0; i < v->size; i++) {
                    ((char *) newV->data)[i] = rt_vector_access_char(v, i, 0);
                }
                break;
            case VECTOR_TYPE_INT:
                newV->data = malloc(sizeof(int64_t) * v->size);
                for (int64_t i = 0; i < v->size; i++) {
                    ((int64_t *) newV->data)[i] = rt_vector_access_int64_t(v, i, 0);
                }
                break;
            case VECTOR_TYPE_FLOAT:
                newV->data = malloc(sizeof(float) * v->size);
                for (int64_t i = 0; i < v->size; i++) {
                    ((float *) newV->data)[i] = rt_vector_access_float(v, i, 0);
                }
                break;
        }
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

