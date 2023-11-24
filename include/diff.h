#ifndef DIFF_H
#define DIFF_H

#include "log.h"
#include "tree.h"
#include "names_table.h"

const double M_ERR = 1e-10;

double TreeCalculate(Tree *const tree);

int TreeTex(Tree *const tree, const char *const file_name);

Tree Derivative(Tree *const tree, const char *const var);

#endif //DIFF_H