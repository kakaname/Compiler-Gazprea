//
// Created by dhanrajbir on 12/11/22.
//

#ifndef GAZPREABASE_TYPEBUILDERPASS_H
#define GAZPREABASE_TYPEBUILDERPASS_H

#include "Types/Type.h"
#include "VisitorPass.h"

struct TypeBuilderPass: VisitorPass<TypeBuilderPass, const Type*> {
    // Implement just tuple, bool, real, int, char.
};


#endif //GAZPREABASE_TYPEBUILDERPASS_H
