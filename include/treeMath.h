#ifndef TREE_MATH_H
#define TREE_MATH_H

#include "log.h"
#include "tree.h"

const double M_ERR = 1e-20;

void FillVariables(VariablesTable *table);

double TreeCalculate(Tree *const tree);

Tree Derivative(Tree *const tree, const char *const var, FILE *file = NULL);

Tree TaylorSeries(Tree *expr, const char *var_name, const double point, const size_t n);

Tree TaylorAproximationPlot(Tree *expr, const char *var_name, const double point, const size_t n, double x_min, double x_max,
                                                                                                  double y_min, double y_max, const char *png_name);
Tree TaylorDifferencePlot(Tree *expr, const char *var_name, const double point, const size_t n, double x_min, double x_max,
                                                                                                double y_min, double y_max, const char *png_name);


int TreeSimplify(Tree *tree, FILE *file = NULL);

#endif //TREE_MATH_H