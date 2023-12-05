#ifndef VARIABLES_H
#define VARIABLES_H

#include <math.h>

#include "log.h"

const size_t MAX_VARIABLES = 100;
const size_t N_CONSTANTS   = 2; //e, pi

struct Variable
{
    char *name;
    double val;
};

struct VariablesTable
{
    size_t size;
    Variable vars[MAX_VARIABLES];
};

Variable *SearchVariable(VariablesTable *const table, const char *const var);

VariablesTable VarsTableCtor(void);

void VarsTableDtor(VariablesTable *table);

#endif //VARIABLES_H
