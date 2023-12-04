#ifndef TREE_I_O_H
#define TREE_I_O_H

#include "tree.h"

enum PlotStatus
{
    START = 0,
    PLOT  = 1,
    END   = 2
};

Tree ReadTree(const char *file_name, VariablesTable *table);


void TreeTextDump(Tree *const tree, FILE *dump_file);


void TreeDot(Tree *const tree, const char *png_file_name);


void SubTreeTex(Node *const node, FILE *tex_file, Node *const parent = NULL);

void TexExprBegin(FILE *tex_file);

void TexExprEnd(FILE *tex_file);

void TexStart(FILE *tex_file);

void TexEnd(FILE *tex_file);

void TexImg(FILE *tex_file, const char *path, const char *message = NULL);

int TreeTex(Tree *const tree, FILE *tex_file);

int TreePlot(PlotStatus status, ...);
#endif //TREE_I_O_H