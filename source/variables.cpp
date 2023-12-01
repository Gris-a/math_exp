#include <stdlib.h>
#include <string.h>

#include "../include/variables.h"

Variable *VariablesParsing(VariablesTable *const table, const char *const var)
{
    ASSERT(table && var, return NULL);

    for(size_t i = 0; i < table->size; i++)
    {
        if(strcmp(table->vars[i].name, var) == 0)
        {
            return table->vars + i;
        }
    }

    return NULL;
}

VariablesTable VarsTableCtor(void)
{
    static char e[] = "e";
    static char pi[] = "pi";

    VariablesTable table = {};

    table.vars[0] = {strdup(e) , M_E};
    table.vars[1] = {strdup(pi), M_PI};
    table.size = 2;

    return table;
}

void VarsTableDtor(VariablesTable *table)
{
    ASSERT(table, return);

    for(size_t i = 0; i < table->size; i++)
    {
        free(table->vars[i].name);
    }
}