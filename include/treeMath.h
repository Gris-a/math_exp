#ifndef TREE_MATH_H
#define TREE_MATH_H

#include "log.h"
#include "tree.h"

const double M_ERR = 1e-20;

void FillVariables(VariablesTable *table);

double TreeCalculate(Tree *const tree);

Tree Derivative(Tree *const tree, const char *const var, FILE *file = NULL);

Tree TaylorSeries(Tree *expr, const char *var_name, const double point, const size_t n);

int TreeSimplify(Tree *tree, FILE *file = NULL);

#endif //TREE_MATH_H