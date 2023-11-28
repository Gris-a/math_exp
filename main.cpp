#include <stdlib.h>

#include "include/tree.h"
#include "include/treeMath.h"
int main(void)
{
    VariablesTable table = VarsTableCtor();
    Tree tree = ReadTree("aboba.txt", &table);
    TREE_DUMP(&tree);
    TreeTex(&tree, LOG_FILE);

    Tree deriv = Derivative(&tree, "x");
    TREE_DUMP(&deriv);
    TreeSimplify(&deriv, 500);
    TREE_DUMP(&deriv);
    TreeTex(&deriv, LOG_FILE);
    TreeDtor(&tree, tree.root);
    TreeDtor(&deriv, deriv.root);
    VarsTableDtor(&table);
}