#ifndef TREE_H
#define TREE_H

#include <stdbool.h>

#include "log.h"
#include "names_table.h"

const size_t BUF_SIZE = 1000;

enum NodePos
{
    LEFT  = -1,
    AUTO  =  0,
    RIGHT =  1
};

enum Operator
{
    ADD = 0,
    SUB = 1,
    MUL = 2,
    DIV = 3,
    POW = 4,
};

enum Function
{
    SIN = 0,
    COS = 1,
    TG  = 2,
    CTG = 3,
    LN  = 5,

};

typedef union
{
    Operator op;
    Function func;
    double   num;
    char    *var;
} data_t;

enum NodeType
{
    UND  = 0,
    VAL  = 1,
    VAR  = 2,
    OP   = 3,
    FUNC = 4
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
    NamesTable *table;

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

Tree ReadTree(const char *file_name, NamesTable *table);

int TreeDtor(Tree *tree, Node *root);

Node *NodeCtor(const data_t val, const NodeType type, Node *const left = NULL, Node *const right = NULL);

int NodeDtor(Node *node);

Node *SubTreeCopy(Node *const node, size_t *counter = NULL);

Node *TreeSearchParent(Tree *const tree, Node *const search_node);

void TreeDot(Tree *const tree, const char *png_file_name);

void TreeDump(Tree *tree, const char *func, const int line);

#ifdef PROTECT
bool IsTreeValid(Tree *const tree);
#endif

#endif //TREE_H