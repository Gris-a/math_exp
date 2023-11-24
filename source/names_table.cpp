#include <stdlib.h>

#include "../include/names_table.h"

void NamesTableDtor(NamesTable *table)
{
    for(size_t i = 2; i < table->size; i++)
    {
        free(table->names[i].name);
    }
}//TODO dump and verificator