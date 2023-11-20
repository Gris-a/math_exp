#ifndef TREE_H
#define TREE_H

#include <stdbool.h>

#include "log.h"

const double M_ERR   = 1e-10;
const int MAX_STR_LEN = 1000;

enum PlacePref
{
    LEFT  = -1,
    AUTO  =  0,
    RIGHT =  1
};

enum NodeType
{
    VAL = 0,
    OP  = 2
};

enum Operator
{
    SUB = 0,
    ADD = 1,
    MUL = 2,
    DIV = 3
};

typedef union
{
    double val;
    Operator op;
} data_t;

struct Node
{
    data_t data;

    NodeType type;

    Node *left;
    Node *right;
};

struct Tree
{
    Node *root;

    size_t size;
};

#define TREE_DUMP(tree_ptr) LOG("%s:%s:%d:\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);\
                            TreeDump(tree_ptr, __func__, __LINE__);\

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

Tree TreeCtor(const data_t init_val);

int TreeDtor(Tree *tree, Node *root);

Node *AddNode(Tree *tree, Node *tree_node, const data_t val, const NodeType type, const PlacePref pref = AUTO);

Node *NodeCtor(const data_t val, const NodeType type, Node *const left = NULL, Node *const right = NULL);

int NodeDtor(Node *node);

Node *TreeSearchParent(Tree *const tree, Node *const search_node);

void TreeTextDump(Tree *const tree, FILE *dump_file = LOG_FILE);

void TreeDot(Tree *const tree, const char *png_file_name);

void TreeDump(Tree *tree, const char *func, const int line);

Tree ReadTree(const char *const file_name);

double TreeCalculate(Tree *const tree);

#ifdef PROTECT
bool IsTreeValid(Tree *const tree);
#endif

#endif //TREE_H