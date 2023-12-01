#include <stdlib.h>
#include <time.h>

#include "include/tree.h"
#include "include/treeMath.h"
#include "include/treeIO.h"

int main(void)
{
    srand((unsigned)time(NULL));
    FILE *TeX_file = fopen("derivative.tex", "w");
    TexStart(TeX_file);

    VariablesTable table = VarsTableCtor();
    Tree tree = ReadTree("aboba.txt", &table);

    //deriv
    Tree deriv = Derivative(&tree, "x", TeX_file);
    TREE_DUMP(&deriv);
    TreeSimplify(&deriv, TeX_file);
    TREE_DUMP(&deriv);

    double point = 1;

    Tree Taylor = TaylorAproximationPlot(&tree, "x", point, 5, 0.0, 3.0, 0.0, 9.0, "aboba.png");
    Tree Differ = TaylorDifferencePlot(&tree, "x", point, 5, 0.0, 3.0, 0.0, 9.0, "abobaboba.png");

    TreeDtor(&tree, tree.root);
    TreeDtor(&deriv, deriv.root);
    TreeDtor(&Taylor, Taylor.root);
    TreeDtor(&Differ, Differ.root);

    VarsTableDtor(&table);

    TexEnd(TeX_file);
    fclose(TeX_file);
}