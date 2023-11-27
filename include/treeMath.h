#ifndef TREE_MATH_H
#define TREE_MATH_H

#include "log.h"
#include "tree.h"

const double M_ERR = 1e-10;

double TreeCalculate(Tree *const tree);

Tree Derivative(Tree *const tree, const char *const var);

int TreeSimplify(Tree *tree, size_t n_iter);

#endif //TREE_MATH_H