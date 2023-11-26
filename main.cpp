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
    Tree deriv2 = Derivative(&deriv, "x");
    TREE_DUMP(&deriv2);
    TreeSimplify(&deriv2, 10000);
    TREE_DUMP(&deriv2);
    TreeDtor(&tree, tree.root);
    TreeDtor(&deriv2, deriv2.root);
    TreeDtor(&deriv, deriv.root);
    VarsTableDtor(&table);
}