#include <stdlib.h>
#include <time.h>

#include "include/tree.h"
#include "include/treeMath.h"
#include "include/treeIO.h"

int main(void)
{
    srand((unsigned)time(NULL));
    FILE *TeX_file = fopen("derivative.tex", "w");

    VariablesTable table = VarsTableCtor();
    Tree tree = ReadTree("aboba.txt", &table);

    //diff
    Tree deriv = Derivative(&tree, "x", TeX_file);
    TreeSimplify(&deriv, TeX_file);

    //Taylor
    Tree Taylor = TaylorSeries(&tree, "x", 0, 5);
    TreeSimplify(&Taylor);
    TreeTex(&Taylor, TeX_file);
    TREE_DUMP(&Taylor);







    TreeDtor(&tree, tree.root);
    TreeDtor(&deriv, deriv.root);
    TreeDtor(&Taylor, Taylor.root);
    VarsTableDtor(&table);
    fclose(TeX_file);
}