//
// Created by rajan on 12/01/22.
//

#include "runtime.h"


MATRIX_ACCESS(char)
MATRIX_ACCESS(int64_t)
MATRIX_ACCESS(float)
MATRIX_SET(char)
MATRIX_SET(int64_t)
MATRIX_SET(float)

struct matrix *rt_matrix_new(int64_t type, u_int64_t rows, u_int64_t cols) {
    struct matrix *m = malloc(sizeof(struct matrix));
    m->rows = rows;
    m->cols = cols;
    m->idx = 0;
    m->type = type;
    m->data = malloc(sizeof(struct vector *) * rows);
    return m;
}

void rt_matrix_empty_rows(struct matrix *m) {
    for (int64_t i = 0; i < m->rows; i++) {
        m->data[i] = rt_vector_new(m->type, m->cols);
    }
}

struct matrix *rt_matrix_create_unpopulated(struct matrix *v) {
    return rt_matrix_new(v->type, v->rows, v->cols);
}

struct matrix *rt_matrix_empty_from_vector(int64_t type, struct vector *row, struct vector *col) {
    struct matrix *m = rt_matrix_new(type, row->size, col->size);
    for (int64_t i = 0; i < row->size; i++) {
        m->data[i] = rt_vector_empty_copy(type, col);
    }
    return m;
}

struct matrix *rt_matrix_create_deep_copy(struct matrix *m) {
    struct matrix *newM = malloc(sizeof(struct matrix));
    newM->rows = m->rows;
    newM->cols = m->cols;
    // In all matrix cases, I found that there is no need to set the idx to anything other
    // than 0
    newM->idx = 0;
    newM->type = m->type;
    newM->data = malloc(sizeof(struct vector *) * m->rows);
    for (int64_t i = 0; i < m->rows; i++) {
        newM->data[i] = rt_vector_create_deep_copy(m->data[i]);
    }
    return newM;
}

void rt_matrix_populate_row(struct matrix *m, struct vector *v, u_int64_t row) {
    // Because of prior casting logic, this runtime function assumes the column size
    // and vector size are equal
    if (row == 0) {
        exit(1);
    }
    row -= 1;
    if (m->cols != v->size) {
        exit(1);
    }

    m->data[row] = rt_vector_create_deep_copy(v);

}
struct matrix *rt_matrix_view_scalar(struct matrix *m, u_int64_t row, u_int64_t col) {

    if (row == 0 || col == 0) {
        exit(1);
    }

    row -= 1;

    struct matrix *newM = malloc(sizeof(struct matrix));

    newM->rows = 1;
    newM->cols = 1;
    newM->idx = 0;
    newM->type = m->type;
    newM->data = malloc(sizeof(struct vector *));
    newM->data[0] = rt_vector_view_scalar(m->data[row], col);
    return newM;
}

struct vector *rt_matrix_vector_get(struct matrix *m, struct vector *domain, int64_t scalar, int64_t orientation) {

    struct vector *v = rt_vector_new(m->type, domain->size);

    for (int64_t i = 0; i < domain->size; i++) {
        if (orientation == 0) {
            // case where the rows item has a vector
            if (m->type == VECTOR_TYPE_INT) {
                rt_vector_set_int64_t(v, i+1, rt_matrix_access_int64_t(m, rt_vector_access_int64_t(domain, i+1, 0), scalar, 0), 0);
            } else if (m->type == VECTOR_TYPE_FLOAT) {
                rt_vector_set_float(v, i+1, rt_matrix_access_float(m, rt_vector_access_int64_t(domain, i+1, 0), scalar, 0), 0);
            } else if (m->type == VECTOR_TYPE_CHAR || m->type == VECTOR_TYPE_BOOL) {
                rt_vector_set_char(v, i+1, rt_matrix_access_char(m, rt_vector_access_int64_t(domain, i+1, 0), scalar, 0), 0);
            }
        } else {
            // case where the cols item has a vector
            if (m->type == VECTOR_TYPE_INT) {
                rt_vector_set_int64_t(v, i+1, rt_matrix_access_int64_t(m, scalar, rt_vector_access_int64_t(domain, i+1, 0), 0), 0);
            } else if (m->type == VECTOR_TYPE_FLOAT) {
                rt_vector_set_float(v, i+1, rt_matrix_access_float(m, scalar, rt_vector_access_int64_t(domain, i+1, 0), 0), 0);
            } else if (m->type == VECTOR_TYPE_CHAR || m->type == VECTOR_TYPE_BOOL) {
                rt_vector_set_char(v, i+1, rt_matrix_access_char(m, scalar, rt_vector_access_int64_t(domain, i+1, 0), 0), 0);
            }
        }
    }
    
    return v;

}

static struct matrix *mv_m = NULL;
static struct vector *mv_domain = NULL;
static int64_t mv_scalar = 0;
static int64_t mv_orientation = 0;

void rt_matrix_vector_init_assign(struct matrix *m, struct vector *domain, int64_t scalar, int64_t orientation) {
    mv_m = m;
    mv_domain = domain;
    mv_scalar = scalar;
    mv_orientation = orientation;
}

void rt_matrix_vector_assign(struct vector *from) {
    struct matrix *m = mv_m;
    struct vector *domain = mv_domain;
    int64_t scalar = mv_scalar;
    int64_t orientation = mv_orientation;

    // This special case is for when we are assigning a vector to a row
    for (int64_t i = 0; i < domain->size; i++) {
        if (orientation == 0) {
            // case where the rows item has a vector
            if (m->type == VECTOR_TYPE_INT) {
                rt_matrix_set_int64_t(m, rt_vector_access_int64_t(domain, i+1, 0), scalar, rt_vector_access_int64_t(from, i+1, 0), 0);
            } else if (m->type == VECTOR_TYPE_FLOAT) {
                rt_matrix_set_float(m, rt_vector_access_int64_t(domain, i+1, 0), scalar, rt_vector_access_float(from, i+1, 0), 0);
            } else if (m->type == VECTOR_TYPE_CHAR || m->type == VECTOR_TYPE_BOOL) {
                rt_matrix_set_char(m, rt_vector_access_int64_t(domain, i+1, 0), scalar, rt_vector_access_char(from, i+1, 0), 0);
            }
        } else {
            // case where the cols item has a vector
            if (m->type == VECTOR_TYPE_INT) {
                rt_matrix_set_int64_t(m, scalar, rt_vector_access_int64_t(domain, i+1, 0), rt_vector_access_int64_t(from, i+1, 0), 0);
            } else if (m->type == VECTOR_TYPE_FLOAT) {
                rt_matrix_set_float(m, scalar, rt_vector_access_int64_t(domain, i+1, 0), rt_vector_access_float(from, i+1, 0), 0);
            } else if (m->type == VECTOR_TYPE_CHAR || m->type == VECTOR_TYPE_BOOL) {
                rt_matrix_set_char(m, scalar, rt_vector_access_int64_t(domain, i+1, 0), rt_vector_access_char(from, i+1, 0), 0);
            }
        }
    }

    // clear the static variables
    mv_m = NULL;
    mv_domain = NULL;
    mv_scalar = 0;
    mv_orientation = 0;


}

struct matrix *rt_matrix_view_vector(struct matrix *m, struct vector *v, u_int64_t scalar, u_int64_t orientation) {

    if (scalar == 0) {
        exit(1);
    }
    scalar -= 1;
    struct matrix *newM = malloc(sizeof(struct matrix));

    // TODO fix case where one of them gets assigned to 1 and we assume vectors can be assigned?
    if (orientation == 0) {
        // if the rows item has a vector, the "hard case"
        newM->rows = v->size;
        newM->cols = 1;
        newM->idx = 0;
        newM->type = m->type;
        newM->data = malloc(sizeof(struct vector *) * v->size);
        for (int64_t i = 0; i < v->size; i++) {
            newM->data[i] = rt_vector_view_scalar(m->data[rt_vector_access_int64_t(v, i+1, 0)-1], scalar);
        }
    } else {
        // if the columns item has a vector, the "easy case"
        newM->rows = 1;
        newM->cols = v->size;
        newM->idx = 0;
        newM->type = m->type;
        newM->data = malloc(sizeof(struct vector *));
        newM->data[0] = rt_vector_view_vector(m->data[scalar], v);
    }

    return newM;
}


struct matrix *rt_matrix_view_matrix(struct matrix *m, struct vector *rows, struct vector *cols) {
    struct matrix *newM = malloc(sizeof(struct matrix));

    newM->rows = rows->size;
    newM->cols = cols->size;
    newM->idx = 0;
    newM->type = m->type;
    newM->data = malloc(sizeof(struct vector *) * rows->size);
    for (int64_t i = 0; i < rows->size; i++) {
        newM->data[i] = rt_vector_view_vector(m->data[rt_vector_access_int64_t(rows, i+1, 0)-1], cols);
    }

    return newM;
}


// TODO this assumes that the vector size matches the column size

// Matrix Operations -- relying on vector ops

struct matrix *rt_matrix_not(struct matrix *a) {
    struct matrix *m = rt_matrix_create_unpopulated(a);
    for (int64_t i = 0; i < a->rows; i++) {
        m->data[i] = rt_vector_not(a->data[i]);
    }
    return m;
}

struct matrix *rt_matrix_sub(struct matrix *a) {
    struct matrix *m = rt_matrix_create_unpopulated(a);
    for (int64_t i = 0; i < a->rows; i++) {
        m->data[i] = rt_vector_sub(a->data[i]);
    }
    return m;
}

struct matrix *rt_matrix_arith(struct matrix *a, struct matrix *b, u_int64_t op) {
    struct matrix *m = rt_matrix_create_unpopulated(a);
    for (int64_t i = 0; i < a->rows; i++) {
        m->data[i] = rt_vector_arith(a->data[i], b->data[i], op);
    }
    return m;
}

struct matrix *rt_matrix_mul(struct matrix *a, struct matrix *b) {
    struct matrix *m = rt_matrix_new(a->type, a->rows, b->cols);

    // loop through each position and create a dot product
    // we should not have any vector views ever here
    for (u_int64_t i = 0; i < a->rows; i++) {
        m->data[i] = rt_vector_new(a->type, b->cols);
        for (u_int64_t j = 0; j < b->cols; j++) {
            if (a->type == VECTOR_TYPE_INT) {
                int64_t sum = 0;
                for (u_int64_t k = 0; k < a->cols; k++) {
                    sum += ((int64_t *) a->data[i]->data)[k] * ((int64_t *) b->data[k]->data)[j];
                }
                ((int64_t *) m->data[i]->data)[j] = sum;
            } else if (a->type == VECTOR_TYPE_FLOAT) {
                float sum = 0;
                for (u_int64_t k = 0; k < a->cols; k++) {
                    sum += ((float *) a->data[i]->data)[k] * ((float *) b->data[k]->data)[j];
                }
                ((float *) m->data[i]->data)[j] = sum;
            } else {
                exit(1);
            }
        }
    }
    return m;
}

struct matrix *rt_matrix_comp(struct matrix *a, struct matrix *b, u_int64_t op) {
    struct matrix *m = rt_matrix_new(VECTOR_TYPE_BOOL, a->rows, a->cols);
    for (int64_t i = 0; i < a->rows; i++) {
        m->data[i] = rt_vector_comp(a->data[i], b->data[i], op);
    }
    return m;
}

struct matrix *rt_matrix_logical(struct matrix *a, struct matrix *b, u_int64_t op) {
    struct matrix *m = rt_matrix_create_unpopulated(a);
    for (int64_t i = 0; i < a->rows; i++) {
        m->data[i] = rt_vector_logical(a->data[i], b->data[i], op);
    }
    return m;
}

u_int8_t rt_matrix_eq(struct matrix *a, struct matrix *b, u_int64_t op) {
    if (a->rows != b->rows || a->cols != b->cols) {
        fprintf(stderr, "Dimensions not compatible for operation\n");
        exit(1);
    }

    for (int64_t i = 0; i < a->rows; i++) {
        struct vector *v = a->data[i];
        struct vector *w = b->data[i];
        if (!rt_vector_eq(v, w, op)) {
            return 0;
        }
    }

    return 1;
}

void rt_vector_copy(struct vector *from, struct vector *to) {
    if (from->size != to->size) {
        exit(1);
    }

    for (int64_t i = 0; i < from->size; i++) {
        switch (from->type) {
            case VECTOR_TYPE_CHAR:
            case VECTOR_TYPE_BOOL:
                rt_vector_set_char(to, i+1, rt_vector_access_char(from, i+1, 0), 0);
                break;
            case VECTOR_TYPE_INT:
                rt_vector_set_int64_t(to, i+1, rt_vector_access_int64_t(from, i+1, 0), 0);
                break;
            case VECTOR_TYPE_FLOAT:
                rt_vector_set_float(to, i+1, rt_vector_access_float(from, i+1, 0), 0);
                break;
        }
    }

}

void rt_matrix_copy(struct matrix *from, struct matrix *to) {

    for (int64_t i = 0; i < from->rows; i++) {
        rt_vector_copy(from->data[i], to->data[i]);
    }
}

void rt_matrix_set_vector(struct matrix *to, struct vector *from) {
    if (to->rows == 1) {
        rt_vector_copy(from, to->data[0]);
    } else if (to->cols == 1) {
        for (int64_t i = 0; i < from->size; i++) {
            switch (from->type) {
                case VECTOR_TYPE_CHAR:
                case VECTOR_TYPE_BOOL:
                    rt_vector_set_char(to->data[i], 0, rt_vector_access_char(from, i+1, 0), 0);
                    break;
                case VECTOR_TYPE_INT:
                    rt_vector_set_int64_t(to->data[i], 0, rt_vector_access_int64_t(from, i+1, 0), 0);
                    break;
                case VECTOR_TYPE_FLOAT:
                    rt_vector_set_float(to->data[i], 0, rt_vector_access_float(from, i+1, 0), 0);
                    break;
            }
        }
    } else {
        exit(1);
    }
}