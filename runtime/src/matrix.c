//
// Created by rajan on 12/01/22.
//

#include "runtime.h"


MATRIX_ACCESS(char)
MATRIX_ACCESS(int)
MATRIX_ACCESS(float)
MATRIX_SET(char)
MATRIX_SET(int)
MATRIX_SET(float)

struct matrix rt_matrix_new(enum vector_type type, u_int32_t rows, u_int32_t cols) {
    struct matrix m;
    m.rows = rows;
    m.cols = cols;
    m.idx = 0;
    m.idx = 0;
    m.type = type;
    m.data = malloc(sizeof(struct vector) * rows);
    return m;
}

struct matrix rt_matrix_create_unpopulated(struct matrix *v) {
    return rt_matrix_new(v->type, v->rows, v->cols);
}

struct matrix rt_matrix_create_deep_copy(struct matrix *m) {
    struct matrix newM;
    newM.rows = m->rows;
    newM.cols = m->cols;
    // In all matrix cases, I found that there is no need to set the idx to anything other
    // than 0
    newM.idx = 0;
    newM.type = m->type;
    newM.data = malloc(sizeof(struct vector) * m->rows);
    for (int i = 0; i < m->rows; i++) {
        newM.data[i] = rt_vector_create_deep_copy(&m->data[i]);
    }
    return newM;
}

void rt_matrix_populate_row(struct matrix *m, struct vector *v, u_int32_t row) {
    // Because of prior casting logic, this runtime function assumes the column size
    // and vector size are equal
    if (m->cols != v->size) {
        exit(1);
    }

    m->data[row] = rt_vector_create_deep_copy(v);

}
struct matrix rt_matrix_view_scalar(struct matrix *m, u_int32_t row, u_int32_t col) {
    struct matrix newM;

    newM.rows = 1;
    newM.cols = 1;
    newM.idx = 0;
    newM.type = m->type;
    struct vector *v = malloc(sizeof(struct vector));
    *v = rt_vector_view_scalar(&m->data[row], col);
    newM.data = v;
    return newM;
}

struct matrix rt_matrix_view_vector(struct matrix *m, struct vector *v, u_int32_t scalar, u_int32_t orientation) {
    struct matrix newM;

    // TODO fix case where one of them gets assigned to 1 and we assume vectors can be assigned?
    if (orientation == 0) {
        // if the rows item has a vector, the "hard case"
        newM.rows = v->size;
        newM.cols = 1;
        newM.idx = 0;
        newM.type = m->type;
        newM.data = malloc(sizeof(struct vector) * v->size);
        for (int i = 0; i < v->size; i++) {
            newM.data[i] = rt_vector_view_scalar(&m->data[rt_vector_access_int(v, i, 0)], scalar);
        }
    } else {
        // if the columns item has a vector, the "easy case"
        newM.rows = 1;
        newM.cols = v->size;
        newM.idx = 0;
        newM.type = m->type;
        newM.data = malloc(sizeof(struct vector));
        newM.data[0] = rt_vector_view_vector(&m->data[scalar], v);
    }

    return newM;
}

struct matrix rt_matrix_view_matrix(struct matrix *m, struct vector *rows, struct vector *cols) {
    struct matrix newM;

    newM.rows = rows->size;
    printf("rows size: %u", rows->size);
    newM.cols = cols->size;
    printf("cols size: %u", cols->size);
    newM.idx = 0;
    newM.type = m->type;
    newM.data = malloc(sizeof(struct vector) * rows->size);
    for (int i = 0; i < rows->size; i++) {
        printf("i is %u", i);
        newM.data[i] = rt_vector_view_vector(&m->data[rt_vector_access_int(rows, i, 0)], cols);
    }

    return newM;
}


// TODO this assumes that the vector size matches the column size



// Matrix Operations -- relying on vector ops

struct matrix rt_matrix_not(struct matrix *a) {
    struct matrix m = rt_matrix_create_unpopulated(a);
    for (int i = 0; i < a->rows; i++) {
        m.data[i] = rt_vector_not(&a->data[i]);
    }
    return m;
}

struct matrix rt_matrix_sub(struct matrix *a) {
    struct matrix m = rt_matrix_create_unpopulated(a);
    for (int i = 0; i < a->rows; i++) {
        m.data[i] = rt_vector_sub(&a->data[i]);
    }
    return m;
}

struct matrix rt_matrix_arith(struct matrix *a, struct matrix *b, u_int32_t op) {
    struct matrix m = rt_matrix_create_unpopulated(a);
    for (int i = 0; i < a->rows; i++) {
        m.data[i] = rt_vector_arith(&a->data[i], &b->data[i], op);
    }
    return m;
}

struct matrix rt_matrix_comp(struct matrix *a, struct matrix *b, u_int32_t op) {
    struct matrix m = rt_matrix_create_unpopulated(a);
    for (int i = 0; i < a->rows; i++) {
        m.data[i] = rt_vector_comp(&a->data[i], &b->data[i], op);
    }
    return m;
}

struct matrix rt_matrix_logical(struct matrix *a, struct matrix *b, u_int32_t op) {
    struct matrix m = rt_matrix_create_unpopulated(a);
    for (int i = 0; i < a->rows; i++) {
        m.data[i] = rt_vector_logical(&a->data[i], &b->data[i], op);
    }
    return m;
}

u_int8_t rt_matrix_eq(struct matrix *a, struct matrix *b, u_int32_t op) {
    if (a->rows != b->rows || a->cols != b->cols) {
        exit(1);
    }

    for (int i = 0; i < a->rows; i++) {
        struct vector v = a->data[i];
        struct vector w = b->data[i];
        if (!rt_vector_eq(&v, &w, op)) {
            return 0;
        }
    }

    return 1;
}

void rt_vector_copy(struct vector *from, struct vector *to) {
    if (from->size != to->size) {
        exit(1);
    }

    for (int i = 0; i < from->size; i++) {
        switch (from->type) {
            case VECTOR_TYPE_CHAR:
            case VECTOR_TYPE_BOOL:
                rt_vector_set_char(to, i, rt_vector_access_char(from, i, 0), 0);
                break;
            case VECTOR_TYPE_INT:
                printf("should");
                rt_vector_set_int(to, i, rt_vector_access_int(from, i, 0), 0);
                break;
            case VECTOR_TYPE_FLOAT:
                rt_vector_set_float(to, i, rt_vector_access_float(from, i, 0), 0);
                break;
        }
    }

}

void rt_matrix_copy(struct matrix *from, struct matrix *to) {

    printf("%d",( (int *)(from->data[0].data) )[0] );

    for (int i = 0; i < from->rows; i++) {
        printf("here");
        rt_vector_copy(&from->data[i], &to->data[i]);
    }
}

void rt_matrix_set_vector(struct matrix *to, struct vector *from) {
    if (to->rows == 1) {
        rt_vector_copy(from, &to->data[0]);
    } else if (to->cols == 1) {
        for (int i = 0; i < from->size; i++) {
            u_int32_t row_idx = i;
            if (to->idx != 0) {
                row_idx = to->idx[i];
            }
            switch (from->type) {
                case VECTOR_TYPE_CHAR:
                case VECTOR_TYPE_BOOL:
                    rt_vector_set_char(&to->data[i], 0, rt_vector_access_char(from, i, 0), 0);
                    break;
                case VECTOR_TYPE_INT:
                    rt_vector_set_int(&to->data[i], 0, rt_vector_access_int(from, i, 0), 0);
                    break;
                case VECTOR_TYPE_FLOAT:
                    rt_vector_set_float(&to->data[i], 0, rt_vector_access_float(from, i, 0), 0);
                    break;
            }
        }
    } else {
        exit(1);
    }
}