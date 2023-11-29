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
    Tree Taylor = TaylorSeries(&tree, "x", 0, 10);
    TreeSimplify(&Taylor);
    TreeTex(&Taylor, TeX_file);

    Tree difference = {};
    difference.root = NodeCtor({SUB}, OP, tree.root, Taylor.root);
    difference.table = tree.table;
    difference.size = 1 + tree.size + Taylor.size;
    TreePlot(&difference, -2, 2);






    TreeDtor(&tree, tree.root);
    TreeDtor(&deriv, deriv.root);
    TreeDtor(&Taylor, Taylor.root);
    NodeDtor(difference.root);
    VarsTableDtor(&table);
    fclose(TeX_file);
}