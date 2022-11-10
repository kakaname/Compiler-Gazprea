//
// Created by rajan on 11/10/22.
//

#pragma once

#include <cstdlib>
#include <cstdio>

void ERR(const char *msg) {
    fprintf(stderr, "%s", msg);
    exit(1);
}

// TODO determine if this is correct/wanted behaviour
void ERR_REAL_NAN() { ERR("Real Number Error: cannot have an invalid number\n"); }
// TODO test case where some tuple's types are inferred to begin with
// FIXME check how this interacts with aliasing
void ERR_TUPLE_MISMATCH() { ERR("Tuple Type Mismatch: the types of two tuples do not match\n");}