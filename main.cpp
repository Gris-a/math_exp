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
    Tree Taylor1 = TaylorSeries(&tree, "x", 0, 1);
    TreeSimplify(&Taylor1);
    Tree Taylor2 = TaylorSeries(&tree, "x", 0, 2);
    TreeSimplify(&Taylor2);
    Tree Taylor3 = TaylorSeries(&tree, "x", 0, 3);
    TreeSimplify(&Taylor3);
    Tree Taylor4 = TaylorSeries(&tree, "x", 0, 4);
    TreeSimplify(&Taylor4);
    Tree Taylor5 = TaylorSeries(&tree, "x", 0, 5);
    TreeSimplify(&Taylor5);


    TreePlot(-2, 2, "plot/gplot.png", 6, &tree, "Function",
                                         &Taylor1, "o(x)",
                                         &Taylor2, "o(x^2)",
                                         &Taylor3, "o(x^3)",
                                         &Taylor4, "o(x^4)",
                                         &Taylor5, "o(x^5)");






    TreeDtor(&tree, tree.root);
    TreeDtor(&deriv, deriv.root);
    TreeDtor(&Taylor1, Taylor1.root);
    TreeDtor(&Taylor2, Taylor2.root);
    TreeDtor(&Taylor3, Taylor3.root);
    TreeDtor(&Taylor4, Taylor4.root);
    TreeDtor(&Taylor5, Taylor5.root);

    VarsTableDtor(&table);
    fclose(TeX_file);
}