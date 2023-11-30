#ifndef TREE_I_O_H
#define TREE_I_O_H

#include "tree.h"

Tree ReadTree(const char *file_name, VariablesTable *table);

void TreeTextDump(Tree *const tree, FILE *dump_file);

void TreeDot(Tree *const tree, const char *png_file_name);

void SubTreeTex(Node *const node, FILE *tex_file, Node *const parent = NULL);

void TexExprBegin(FILE *tex_file);

void TexExprEnd(FILE *tex_file);

int TreeTex(Tree *const tree, FILE *tex_file);

//variadic arguments is Tree * to plot and char * title of expression
int TreePlot(const double lx_bound, const double rx_bound, const double ly_bound, const double ry_bound, char *plot_name, const unsigned num_expr, ...);
#endif //TREE_I_O_H