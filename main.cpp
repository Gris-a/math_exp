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
    TREE_DUMP(&deriv);
    TreeSimplify(&deriv, TeX_file);
    TREE_DUMP(&deriv);

    double point = 1;

    //Taylor
    Tree Taylor1 = TaylorSeries(&tree, "x", point, 1);
    TreeSimplify(&Taylor1);
    Tree Taylor2 = TaylorSeries(&tree, "x", point, 2);
    TreeSimplify(&Taylor2);
    Tree Taylor3 = TaylorSeries(&tree, "x", point, 3);
    TreeSimplify(&Taylor3);
    Tree Taylor4 = TaylorSeries(&tree, "x", point, 4);
    TreeSimplify(&Taylor4);
    Tree Taylor5 = TaylorSeries(&tree, "x", point, 5);
    TreeSimplify(&Taylor5);


    TreePlot(0, 3,
             0, 10, NULL, 6, &tree   , "Function",
                                 &Taylor1, "o((x - 1))",
                                 &Taylor2, "o((x - 1)^2)",
                                 &Taylor3, "o((x - 1)^3)",
                                 &Taylor4, "o((x - 1)^4)",
                                 &Taylor5, "o((x - 1)^5)");






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