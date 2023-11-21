#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../include/tree.h"

Tree TreeCtor(const data_t init_val, const NodeType type)
{
    Node *root = NodeCtor(init_val, type);
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

int TreeDtor(Tree *tree)
{
    TREE_VERIFICATION(tree, EXIT_FAILURE);

    SubTreeDtor(tree, tree->root);
    tree->root = NULL;

    memset(tree->labels, 0, tree->n_labels);
    tree->n_labels = 0;

    return EXIT_SUCCESS;
}


Node *TreeAdd(Tree *tree, Node *tree_node, const data_t val, const NodeType type, const PlacePref pref)
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
                if(val.val <= (*next)->data.val) next = &((*next)->left );
                else                             next = &((*next)->right);
                break;
            default: return NULL;
        }
    }

    (*next) = NodeCtor(val, type);
    ASSERT((*next), return NULL);

    tree->size++;

    return (*next);
}

Node *TreeAddNode(Tree *tree, Node *tree_node, Node *const add_node, const PlacePref pref)
{
    TREE_VERIFICATION(tree, NULL);

    ASSERT(tree_node, return NULL);
    ASSERT(add_node , return NULL);

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
                if(add_node->data.val <= (*next)->data.val) next = &((*next)->left );
                else                                        next = &((*next)->right);
                break;
            default: return NULL;
        }
    }

    (*next) = add_node;
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


static NodeType ReadData(char **buffer, data_t *data, Label labels[], size_t *n_labels)
{
    int offset = 0;
    NodeType ans  = VAL;

    if(!sscanf(*buffer, " %lg%n", &data->val, &offset))
    {
        char ch = 0;
        sscanf(*buffer, " %c%n", &ch, &offset);

        ans = OP;
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
            default:
            {
                data->var = ch;

                bool exists = false;
                for(size_t i = 0; i < *n_labels; i++)
                {
                    if(labels[i].name == ch)
                    {
                        exists = true;
                        break;
                    }
                }

                if(!exists)
                {
                    ASSERT(*n_labels < MAX_LABELS, return VAR);
                    labels[(*n_labels)++].name = ch;
                }

                ans = VAR;
                break;
            }
        }
    }

    (*buffer) += offset;

    return ans;
}

static Node *ReadSubTree(char *tree_buf, size_t *counter, Label labels[], size_t *n_labels)
{
    static char *buffer = tree_buf;
    static int offset   = 0;
    static char ch      = 0;

    sscanf(buffer, " %c%n", &ch, &offset);
    buffer += offset;

    switch(ch)
    {
        case '(':
        {
            Node *left  = ReadSubTree(buffer, counter, labels, n_labels);

            data_t data = {};
            NodeType type = ReadData(&buffer, &data, labels, n_labels);

            Node *right = ReadSubTree(buffer, counter, labels, n_labels);

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
    int exit_status = stat(file_name, &file_info);
    ASSERT(exit_status != EOF, return ULLONG_MAX);

    return (size_t)file_info.st_size;
}

Tree ReadTree(const char *const file_name)
{
    size_t file_size = FileSize(file_name);
    ASSERT(file_size != ULLONG_MAX, return {});

    FILE *file = fopen(file_name, "rb");
    ASSERT(file, return {});

    char *buffer = (char *)calloc(file_size + 1, sizeof(char));
    ASSERT(buffer, fclose(file); return {});

    fread(buffer, file_size, sizeof(char), file);
    fclose(file);

    size_t counter = 0;
    Tree tree      = {};

    tree.root = ReadSubTree(buffer, &counter, tree.labels, &tree.n_labels);
    tree.size = counter;

    free(buffer);

    return tree;
}


static Label *LabelsParsing(Tree *const tree, const char label)
{
    for(size_t i = 0; i < tree->n_labels; i++)
    {
        if(label == tree->labels[i].name) return tree->labels + i;
    }

    return NULL;
}


static double SubTreeCalculate(Tree *const tree, Node *const node)
{
    ASSERT(node, return NAN);

    switch(node->type)
    {
        case VAL: return node->data.val;
        case OP:
        {
            double calc_left  = SubTreeCalculate(tree, node->left );
            double calc_right = SubTreeCalculate(tree, node->right);

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
                    if(abs(calc_right) < M_ERR)
                    {
                        LOG("Division by zero.\n");
                        return NAN;
                    }
                    return calc_left / calc_right;
                }
                default:
                {
                    LOG("Unknown operator.\n");
                    return NAN;
                }
            }
        }
        case VAR:
        {
            Label *var = LabelsParsing(tree, node->data.var);
            ASSERT(var, return NAN);

            return var->val;
        }
        default:
        {
            LOG("Unknown node type.\n");
            return NAN;
        }
    }

    return NAN;
}

double TreeCalculate(Tree *const tree)
{
    TREE_VERIFICATION(tree, NAN);

    return SubTreeCalculate(tree, tree->root);
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
                default:
                    fputc('?', dump_file);
                    return;
            }
        }
        case VAL:
        {
            fprintf(dump_file, "%lg", tree_node->data.val);
            return;
        }
        case VAR:
        {
            fputc(tree_node->data.var, dump_file);
            return;
        }
        default:
        {
            fprintf(dump_file, "<unknown type>");
            return;
        }
    }
}


static int OpCmp(const Operator type1, const Operator type2)
{
    if(type1 == DIV)
    {
       return  1;
    }
    else if(type1 == ADD)
    {
        return -1;
    }
    else if(type1 == MUL || type1 == SUB)
    {
        if(type2 == ADD || type2 == SUB)
        {
            return 1;
        }

        return -1;
    }

    return 0;
}

static void SubTreeTextDump(Node *const tree_node, const data_t parent_data, FILE *dump_file)
{
    if(!tree_node) return;

    int op_cmp = 0;
    if(tree_node->type == OP)
    {
        op_cmp = OpCmp(parent_data.op, tree_node->data.op);
    }
    else op_cmp = -1;

    ASSERT(op_cmp, return);

    if(op_cmp > 0)
    {
        fprintf(dump_file, "(");
    }

    SubTreeTextDump(tree_node->left, tree_node->data, dump_file);

    NodeDataDump(dump_file, tree_node);

    SubTreeTextDump(tree_node->right, tree_node->data, dump_file);

    if(op_cmp > 0)
    {
        fprintf(dump_file, ")");
    }
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

    SubTreeTextDump(tree->root, {0}, dump_file);
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
        case VAR:
            fprintf(dot_file , "\"blue\"];");
            break;
        default:
            fprintf(dot_file , "\"orange\"];");
            break;
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
    ASSERT(png_file_name, return);

    if(!(tree && tree->root)) return;

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


static void MakeDumpDir(void)
{
    system("rm -rf dump_tree");
    system("mkdir dump_tree");
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

#ifdef PROTECT
static void TreeSizeValidation(Tree *const tree, Node *const tree_node, size_t *counter)
{
    if(!tree_node || (*counter) >= tree->size) return;

    if(tree_node->type == VAR)
    {
        if(!LabelsParsing(tree, tree_node->data.var)) return;
    }

    (*counter)++;

    TreeSizeValidation(tree, tree_node->left , counter);
    TreeSizeValidation(tree, tree_node->right, counter);
}

bool IsTreeValid(Tree *const tree)
{
    ASSERT(tree && tree->root    , return false);
    ASSERT(tree->size <= UINT_MAX, return false);

    ASSERT(tree->n_labels <= MAX_LABELS, return false);
    for(size_t i = 0; i < tree->n_labels; i++)
    {
        ASSERT(tree->labels[i].name, return false);
    }

    size_t counter = 0;
    TreeSizeValidation(tree, tree->root, &counter);

    ASSERT(counter == tree->size, return false);

    return true;
}
#endif