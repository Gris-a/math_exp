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

    TreePlot(START, 0.0, 3.0, 0.0, 9.0, "plot/func.png");
    TreePlot(PLOT, &tree, NULL);
    TreePlot(END);
    TexImg(TeX_file, "plot/func.png", "График функции::");



    //deriv
    Tree deriv = Derivative(&tree, "x", TeX_file);
    TREE_DUMP(&deriv);
    TreeSimplify(&deriv, TeX_file);
    TREE_DUMP(&deriv);

    TreePlot(START, 0.0, 3.0, 0.0, 9.0, "plot/derivative.png");
    TreePlot(PLOT, &deriv, "derivative");
    TreePlot(END);
    TexImg(TeX_file, "plot/derivative.png", "График производной:");



    double point = 1;

    Tree Taylor = TaylorAproximationPlot(&tree, "x", point, 5, 0.0, 3.0, 0.0, 9.0, "plot/taylor.png");
    TexImg(TeX_file, "plot/taylor.png", "Разложение по Тейлору:");

    Tree Differ = TaylorDifferencePlot(&tree, "x", point, 5, 0.0, 3.0, 0.0, 9.0, "plot/diff.png");
    TexImg(TeX_file, "plot/diff.png", "Разница с разложением по Тейлору:");



    TreeDtor(&tree, tree.root);
    TreeDtor(&deriv, deriv.root);
    TreeDtor(&Taylor, Taylor.root);
    TreeDtor(&Differ, Differ.root);

    VarsTableDtor(&table);

    TexEnd(TeX_file);
    fclose(TeX_file);
}