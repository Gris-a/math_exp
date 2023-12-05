#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/tree.h"
#include "../include/treeIO.h"

static Node *SubTreeSearchParent(Node *const node, Node *const search_node);
static void SubTreeSizeCounter(Node *sub_tree, size_t *counter);

static void MakeDumpDir(void);


static Node *SubTreeSearchParent(Node *const node, Node *const search_node)
{
    if(!node) return NULL;
    else if(node->left  == search_node ||
            node->right == search_node) return node;

    Node *find  = SubTreeSearchParent(node->left , search_node);

    if(find == NULL)
    {
        find = SubTreeSearchParent(node->right, search_node);
    }

    return find;
}

void SubTreeDtor(Node *sub_tree, Tree *tree)
{
    if(!sub_tree) return;

    SubTreeDtor(sub_tree->left , tree);
    SubTreeDtor(sub_tree->right, tree);

    NodeDtor(sub_tree);
    if(tree) tree->size--;
}

int TreeDtor(Tree *tree, Node *root)
{
    TREE_VERIFICATION(tree, EXIT_FAILURE);

    if(!root) return EXIT_SUCCESS;

    if(root->left)
    {
        SubTreeDtor(root->left, tree);
        root->left  = NULL;
    }
    if(root->right)
    {
        SubTreeDtor(root->right, tree);
        root->right = NULL;
    }

    if(root == tree->root)
    {
        tree->root = NULL;
    }
    else
    {
        Node *parent = SubTreeSearchParent(tree->root, root);

        if(parent->left == root) parent->left  = NULL;
        else                     parent->right = NULL;
    }

    NodeDtor(root);
    tree->size--;

    return EXIT_SUCCESS;
}


Node *NodeCtor(const data_t val, const NodeType type, Node *const left, Node *const right)
{
    Node *node = (Node *)calloc(1, sizeof(Node));
    ASSERT(node, return NULL);

    node->type  = type;
    node->data  = val;

    node->left  = left;
    node->right = right;

    return node;
}

int NodeDtor(Node *node)
{
    ASSERT(node, return EXIT_FAILURE);
    free(node);

    return EXIT_SUCCESS;
}


Node *SubTreeCopy(Node *sub_tree)
{
    if(!sub_tree) return NULL;
    return NodeCtor(sub_tree->data, sub_tree->type, SubTreeCopy(sub_tree->left),
                                                    SubTreeCopy(sub_tree->right));
}


static void SubTreeSizeCounter(Node *sub_tree, size_t *counter)
{
    if(!sub_tree) return;
    (*counter)++;

    SubTreeSizeCounter(sub_tree->left , counter);
    SubTreeSizeCounter(sub_tree->right, counter);
}

size_t SubTreeSize(Node *sub_tree)
{
    size_t size = 0;
    SubTreeSizeCounter(sub_tree, &size);

    return size;
}



static void MakeDumpDir(void)
{
    system("rm -rf dump_tree");
    system("mkdir dump_tree");
}

void TreeDump(Tree *tree, const char *func, const int line)
{
    static int num = 0;

    if(num == 0) MakeDumpDir();

    TreeTextDump(tree, LOG_FILE);

    char *file_name = NULL;

    asprintf(&file_name, "dump_tree/tree_dump%d__%s:%d__.png", num, func, line);
    TreeDot(tree, file_name);
    free(file_name);

    num++;
}

#ifdef PROTECT
static void TreeValidation(Tree *const tree, Node *const node, size_t *counter)
{
    if(!(node) || (*counter) > tree->size) return;

    if(node->type == VAR)
    {
        ASSERT(node->data.var, return);
        ASSERT(SearchVariable(tree->table, node->data.var), return);
    }

    if(node->type != OP)
    {
        ASSERT(!node->left && !node->right, return);
    }

    (*counter)++;

    TreeValidation(tree, node->left , counter);
    TreeValidation(tree, node->right, counter);
}

bool IsTreeValid(Tree *const tree)
{
    ASSERT(tree && tree->root   , return false);
    ASSERT(tree->size <= INT_MAX, return false);
    ASSERT(tree->table          , return false);

    size_t counter = 0;
    TreeValidation(tree, tree->root, &counter);

    ASSERT(counter == tree->size, return false);

    return true;
}
#endif