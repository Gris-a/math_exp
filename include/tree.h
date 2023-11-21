#ifndef TREE_H
#define TREE_H

#include <stdbool.h>

#include "log.h"
#include "structures.h"

const double M_ERR   = 1e-10;
const int MAX_STR_LEN = 1000;

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

Tree TreeCtor(const data_t init_val, const NodeType type);

int TreeDtor(Tree *tree);

Node *TreeAdd(Tree *tree, Node *tree_node, const data_t val, const NodeType type, const PlacePref pref = AUTO);

Node *TreeAddNode(Tree *tree, Node *tree_node, Node *const add_node, const PlacePref pref = AUTO);

Node *NodeCtor(const data_t val, const NodeType type, Node *const left = NULL, Node *const right = NULL);

int NodeDtor(Node *node);

Node *TreeSearchParent(Tree *const tree, Node *const search_node);

void TreeDot(Tree *const tree, const char *png_file_name);

void TreeDump(Tree *tree, const char *func, const int line);

void TreeTex(Tree *const tree, const char *const message, FILE *dump_file);

Tree ReadTree(const char *const file_name);

double TreeCalculate(Tree *const tree);

#ifdef PROTECT
bool IsTreeValid(Tree *const tree);
#endif

#endif //TREE_H