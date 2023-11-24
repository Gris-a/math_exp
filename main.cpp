#include <stdlib.h>

#include "include/tree.h"
#include "include/diff.h"

int main(void)
{
    NamesTable table = {};
    Tree tree = ReadTree("aboba.txt", &table);
    TREE_DUMP(&tree);
    printf("%lg\n", TreeCalculate(&tree));
    TreeTex(&tree, "tree.tex");
    // Tree deriv = Derivative(&tree, "x");
    // TREE_DUMP(&deriv);
    TreeDtor(&tree, tree.root);
    // TreeDtor(&deriv, deriv.root);
    NamesTableDtor(&table);
}