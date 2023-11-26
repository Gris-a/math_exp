#include <stdlib.h>
#include <string.h>

#include "../include/variables.h"

VariablesTable VarsTableCtor(void)
{
    static char e[] = "e";
    static char pi[] = "pi";

    VariablesTable table = {};

    table.vars[0].name = strdup(e);
    table.vars[1].name = strdup(pi);
    table.size = 2;

    return table;
}

void VarsTableDtor(VariablesTable *table)
{
    for(size_t i = 0; i < table->size; i++)
    {
        free(table->vars[i].name);
    }
}