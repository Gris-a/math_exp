#ifndef TREE_H
#define TREE_H

#include <stdbool.h>

#include "log.h"
#include "variables.h"

const size_t BUF_SIZE = 1000;

#define DEF_OP(enum, literal, eval, dif, smpl) enum,
enum Operator
{
    #include "../include/Operators.h"
};
#undef DEF_OP

typedef union
{
    Operator op;
    double   num;
    char    *var;
} data_t;

enum NodeType
{
    UND  = 0,
    VAL  = 1,
    VAR  = 2,
    OP   = 3,
};

struct Node
{
    NodeType type;
    data_t   data;

    Node *left;
    Node *right;
};

struct Tree
{
    Node *root;
    VariablesTable *table;

    size_t size;
};

#define TREE_DUMP(tree_ptr) LOG("Called from %s:%s:%d:\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);\
                            TreeDump(tree_ptr, __func__, __LINE__);

#ifdef PROTECT
#define TREE_VERIFICATION(tree_ptr, ret_val_on_fail) if(!IsTreeValid(tree_ptr))\
                                                     {\
                                                         LOG("%s:%s:%d: Error: invalid tree.\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);\
                                                         TREE_DUMP(tree_ptr);\
                                                         return ret_val_on_fail;\
                                                     }
#else
#define TREE_VERIFICATION(tree_ptr, ...)
#endif

Tree ReadTree(const char *file_name, VariablesTable *table);

int TreeDtor(Tree *tree, Node *root);

Node *NodeCtor(const data_t val, const NodeType type, Node *const left = NULL, Node *const right = NULL);

int NodeDtor(Node *node);

void TreeDump(Tree *tree, const char *func, const int line);

#ifdef PROTECT
bool IsTreeValid(Tree *const tree);
#endif

#endif //TREE_H