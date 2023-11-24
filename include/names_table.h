#ifndef NAME_TABLE_H
#define NAME_TABLE_H

#include <math.h>

const size_t MAX_NAMES = 100;

struct Name
{
    char *name;
    double val;
};

struct NamesTable
{
    size_t size = 2;
    Name names[MAX_NAMES] = {{"e", M_E}, {"pi", M_PI}};
};

void NamesTableDtor(NamesTable *table);

#endif //NAME_TABLE_H
