#include <stdlib.h>

#include "include/tree.h"
#include "include/treeMath.h"

int main(void)
{
    VariablesTable table = VarsTableCtor();
    Tree tree = ReadTree("aboba.txt", &table);
    TREE_DUMP(&tree);
    printf("%lg\n", TreeCalculate(&tree));
    Tree deriv = Derivative(&tree, "x");
    TREE_DUMP(&deriv);
    TreeSimplify(&deriv, 5);
    TREE_DUMP(&deriv);
    TreeDtor(&tree, tree.root);
    TreeDtor(&deriv, deriv.root);
    VarsTableDtor(&table);
}