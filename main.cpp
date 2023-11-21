#include "include/tree.h"

int main(void)
{
    Tree tree = ReadTree("aboba.txt");
    TREE_DUMP(&tree);

    printf("%lg", TreeCalculate(&tree));

    TreeDtor(&tree);
}