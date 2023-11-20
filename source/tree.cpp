#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../include/tree.h"

Tree TreeCtor(const data_t init_val)
{
    Node *root = NodeCtor(init_val);

    ASSERT(root, return {});

    Tree tree = {root, 1};

    return tree;
}


static void SubTreeDtor(Tree *tree, Node *sub_tree)
{
    if(!sub_tree) return;

    SubTreeDtor(tree, sub_tree->left );
    SubTreeDtor(tree, sub_tree->right);

    NodeDtor(sub_tree);

    tree->size--;
}

int TreeDtor(Tree *tree, Node *root)
{
    TREE_VERIFICATION(tree, EXIT_FAILURE);

    ASSERT(root, return EXIT_FAILURE);

    SubTreeDtor(tree, root->left);
    root->left  = NULL;

    SubTreeDtor(tree, root->right);
    root->right = NULL;

    if(root == tree->root)
    {
        tree->root = NULL;
    }
    else
    {
        Node *parent = TreeSearchParent(tree, root);
        if(parent->left == root) parent->left  = NULL;
        else                     parent->right = NULL;
    }

    NodeDtor(root);

    tree->size--;

    return EXIT_SUCCESS;
}


Node *AddNode(Tree *tree, Node *tree_node, const data_t val, const NodeType type, const PlacePref pref)
{
    TREE_VERIFICATION(tree, NULL);

    ASSERT(tree_node, return NULL);

    Node **next = &tree_node;
    while(*next)
    {
        switch(pref)
        {
            case LEFT:
                next = &((*next)->left);
                break;
            case RIGHT:
                next = &((*next)->right);
                break;
            case AUTO:
                if(val.val <= 0) next = &((*next)->left );
                else             next = &((*next)->right);
                break;
            default: return NULL;
        }
    }

    (*next) = NodeCtor(val, type);

    ASSERT((*next), return NULL);

    tree->size++;

    return (*next);
}


Node *NodeCtor(const data_t val, const NodeType type, Node *const left, Node *const right)
{
    Node *node = (Node *)calloc(1, sizeof(Node));

    ASSERT(node, return NULL);

    node->data  = val;
    node->type  = type;
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


static Node *SubTreeSearchParent(Node *const tree_node, Node *const search_node)
{
    if(!tree_node) return NULL;
    else if(tree_node->left  == search_node ||
            tree_node->right == search_node) return tree_node;

    Node *find  = SubTreeSearchParent(tree_node->left , search_node);

    return (find ? find : SubTreeSearchParent(tree_node->right, search_node));
}

Node *TreeSearchParent(Tree *const tree, Node *const search_node)
{
    TREE_VERIFICATION(tree, NULL);

    ASSERT(search_node, return NULL);

    return SubTreeSearchParent(tree->root, search_node);
}


static NodeType ReadData(char **buffer, data_t *data)
{
    int offset = 0;
    NodeType ans  = VAL;

    if(!sscanf(*buffer, " %lg%n", &data->val, &offset))
    {
        char ch = 0;
        sscanf(*buffer, " %c%n", &ch, &offset);

        switch(ch)
        {
            case '+':
                data->op = ADD;
                break;
            case '-':
                data->op = SUB;
                break;
            case '*':
                data->op = MUL;
                break;
            case '/':
                data->op = DIV;
                break;
            default: break; //TODO variable
        }

        ans = OP;
    }

    (*buffer) += offset;

    return ans;
}

static Node *ReadSubTree(char *buf, size_t *counter)
{
    static char *buffer = NULL;

    buffer = buf;
    int offset = 0;

    char ch = 0;
    sscanf(buffer, " %c%n", &ch, &offset);
    buffer += offset;

    switch(ch)
    {
        case '(':
        {
            Node *left  = ReadSubTree(buffer, counter);

            data_t data = {};
            NodeType type = ReadData(&buffer, &data);

            Node *right = ReadSubTree(buffer, counter);

            sscanf(buffer, " %c%n", &ch, &offset);
            buffer += offset;

            if(ch != ')')
            {
                LOG("Invalid data.\n");

                NodeDtor(left );
                NodeDtor(right);

                return NULL;
            }

            (*counter)++;

            return NodeCtor(data, type, left, right);
        }
        default:
        {
            buffer--;

            return NULL;
        }
    }
}

static size_t FileSize(const char *file_name)
{
    struct stat file_info = {};
    stat(file_name, &file_info);

    return (size_t)file_info.st_size;
}

Tree ReadTree(const char *const file_name)
{
    FILE *file = fopen(file_name, "rb");
    ASSERT(file, return {});

    size_t file_size = FileSize(file_name) + 1;
    char *buffer = (char *)calloc(file_size, sizeof(char));
    fread(buffer, file_size, sizeof(char), file);

    fclose(file);

    size_t counter = 0;
    Tree tree      = {};

    tree.root = ReadSubTree(buffer, &counter);
    tree.size = counter;

    free(buffer);

    return tree;
}


static void MakeDumpDir(void)
{
    system("rm -rf dump_tree");
    system("mkdir dump_tree");
}


static void NodeDataDump(FILE *dump_file, Node *const tree_node)
{
    switch(tree_node->type)
    {
        case OP:
        {
            switch(tree_node->data.op)
            {
                case ADD:
                    fputc('+', dump_file);
                    return;
                case SUB:
                    fputc('-', dump_file);
                    return;
                case MUL:
                    fputc('*', dump_file);
                    return;
                case DIV:
                    fputc('/', dump_file);
                    return;
                default: return; //TODO error
            }
        }
        case VAL:
        {
            fprintf(dump_file, "%lg", tree_node->data.val);
            return;
        }
        default: return; //TODO variables
    }
}

static void SubTreeTextDump(Node *const tree_node, FILE *dump_file)
{
    if(!tree_node) return;

    if(tree_node->left) fprintf(dump_file, "("); //TODO skobki proverka
    SubTreeTextDump(tree_node->left , dump_file);

    NodeDataDump(dump_file, tree_node);

    SubTreeTextDump(tree_node->right, dump_file);
    if(tree_node->right) fprintf(dump_file, ")");
}

void TreeTextDump(Tree *const tree, FILE *dump_file)
{
    ASSERT(dump_file, return);

    if(!tree) return;

    if(dump_file == LOG_FILE)
    {
        LOG("TREE[%p]:  \n"
            "\troot: %p \n"
            "\tsize: %zu\n", tree, tree->root, tree->size);
    }

    if(!tree->root) return;

    SubTreeTextDump(tree->root, dump_file);
    fprintf(dump_file, "\n\n");
}


static void DotNodeCtor(Node *const node, FILE *dot_file)
{
    fprintf(dot_file, "node%p[label = \"{<data>", node);
    NodeDataDump(dot_file, node);

    fprintf(dot_file, "| {<left> left | <right> right}}\"; fillcolor = ");

    switch(node->type)
    {
        case VAL:
            fprintf(dot_file , "\"orchid\"];");
            break;
        case OP:
            fprintf(dot_file , "\"green\"];");
            break;
        default: break; //TODO variables
    }
}

static void DotTreeCtor(Node *const node, Node *const node_next, const char *direction, FILE *dot_file)
{
    if(!node_next) return;

    DotNodeCtor(node_next, dot_file);

    fprintf(dot_file, "\nnode%p:<%s>:s -> node%p:<data>:n;\n", node, direction, node_next);

    DotTreeCtor(node_next, node_next->left , "left" , dot_file);
    DotTreeCtor(node_next, node_next->right, "right", dot_file);
}

static void TreeDotGeneral(Tree *const tree, FILE *dot_file)
{
    fprintf(dot_file, "digraph\n"
                      "{\n"
                      "bgcolor = \"grey\";\n"
                      "ranksep = \"equally\";\n"
                      "node[shape = \"Mrecord\"; style = \"filled\"; fillcolor = \"#58CD36\"];\n"
                      "{rank = source;");
    fprintf(dot_file, "nodel[label = \"<root> root: %p | <size> size: %zu\"; fillcolor = \"lightblue\"]; ",
                                                                                  tree->root, tree->size);
    DotNodeCtor(tree->root, dot_file);

    fprintf(dot_file, "};\n");
}

void TreeDot(Tree *const tree, const char *png_file_name)
{
    if(!(tree && tree->root)) return;

    ASSERT(png_file_name, return);

    FILE *dot_file = fopen("tree.dot", "wb");
    ASSERT(dot_file, return);

    TreeDotGeneral(tree, dot_file);

    DotTreeCtor(tree->root, tree->root->left , "left" , dot_file);
    DotTreeCtor(tree->root, tree->root->right, "right", dot_file);

    fprintf(dot_file, "}\n");

    fclose(dot_file);

    char sys_cmd[MAX_STR_LEN] = {};
    sprintf(sys_cmd, "dot tree.dot -T png -o %s", png_file_name);
    system(sys_cmd);

    remove("tree.dot");
}


void TreeDump(Tree *tree, const char *func, const int line)
{
    static int num = 0;

    if(num == 0) MakeDumpDir();

    char file_name[MAX_STR_LEN] = {};

    TreeTextDump(tree);

    sprintf(file_name, "dump_tree/tree_dump%d__%s:%d__.png", num, func, line);
    TreeDot(tree, file_name);

    num++;
}


static double SubTreeCalculate(Node *const node)
{
    if(!node) return NAN;

    switch(node->type)
    {
        case VAL: return node->data.val;
        case OP:
        {
            double calc_left  = SubTreeCalculate(node->left );
            double calc_right = SubTreeCalculate(node->right);

            switch(node->data.op)
            {
                case ADD:
                {
                    return calc_left + calc_right;
                }
                case SUB:
                {
                    return calc_left - calc_right;
                }
                case MUL:
                {
                    return calc_left * calc_right;
                }
                case DIV:
                {
                    ASSERT(abs(calc_right) > M_ERR, return NAN);
                    return calc_left / calc_right;
                }
                default: return NAN; //TODO error
            }
        }
        default: return NAN; //TODO variable
    }

    return NAN;
}

double TreeCalculate(Tree *const tree)
{
    TREE_VERIFICATION(tree, NAN);

    return SubTreeCalculate(tree->root);
}


#ifdef PROTECT
static void TreeSizeValidation(Tree *const tree, Node *const tree_node, size_t *counter)
{
    if(!tree_node || (*counter) >= tree->size) return;

    (*counter)++;

    TreeSizeValidation(tree, tree_node->left , counter);
    TreeSizeValidation(tree, tree_node->right, counter);
}

bool IsTreeValid(Tree *const tree)
{
    ASSERT(tree && tree->root    , return false);
    ASSERT(tree->size <= UINT_MAX, return false);

    size_t counter = 0;
    TreeSizeValidation(tree, tree->root, &counter);

    ASSERT(counter == tree->size, return false);

    return true;
}
#endif