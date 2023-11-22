#include "include/tree.h"

int main(void)
{
    Tree tree = ReadTree("aboba.txt");
    TREE_DUMP(&tree);
    FILE *dump = fopen("main.tex", "wb");
    TreeTex(&tree, NULL, dump);
    fclose(dump);

    TreeCalculate(&tree);
    TREE_DUMP(&tree);

    TreeDtor(&tree);
}