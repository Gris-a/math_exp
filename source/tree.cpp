#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../include/tree.h"

static NodeType VariablesProcessing(Tree *tree, data_t *data, char *const var_name)
{
    bool exists = false;
    for(size_t i = 0; i < tree->table->size; i++)
    {
        if(strcmp(tree->table->vars[i].name, var_name) == 0)
        {
            data->var = tree->table->vars[i].name;
            exists    = true;
            break;
        }
    }

    if(!exists)
    {
        if(tree->table->size >= MAX_VARIABLES) return UND;

        tree->table->vars[tree->table->size].name = strdup(var_name);
        data->var = tree->table->vars[tree->table->size++].name;
    }

    return VAR;
}

static NodeType OperatorsProcessing(data_t *data, const char *op_name)
{
#define DEF_OP(e_name, e_code, dump, ...) if(strcmp(op_name, dump) == 0)\
                                          {data->op = e_name;} else
    #include "../include/Operators.h"
    /*else*/ return UND;
#undef DEF_OP

    return OP;
}

static NodeType ReadData(char **buf, Tree *tree, data_t *data)
{
    NodeType ans = VAL;
    int offset   = 0;

    int scaned = 0;
    scaned = sscanf(*buf, " %lg %n", &data->num, &offset);
    if(!scaned)
    {
        ans = OP;
        char ch = 0;
        char name_buf[BUF_SIZE] = {};

        char fmt[BUF_SIZE] = {};
        sprintf(fmt, " %%%zu[^() \n\r\t\v\f] %%n%%c", BUF_SIZE - 1);

        sscanf(*buf, fmt, name_buf, &offset, &ch);
        switch(ch)
        {
            case '(':
            {
                ans = OperatorsProcessing(data, name_buf);
                break;
            }
            case ')':
            {
                ans = VariablesProcessing(tree, data, name_buf);
                break;
            }
            default:
            {
                return UND;
            }
        }
    }

    *buf += offset;

    return ans;
}

static Node *ReadSubTree(char *const buffer, Tree *tree)
{
    static char *buf = buffer;

    int offset = 0;
    char ch    = 0;

    sscanf(buf, " %c%n", &ch, &offset);
    buf += offset;

    switch(ch)
    {
        case '(':
        {
            Node *left  = ReadSubTree(buf, tree);

            data_t data = {};
            NodeType type = ReadData(&buf, tree, &data);
            if(type == UND)
            {
                LOG("Invalid data.\n");
                NodeDtor(left);
                return NULL;
            }

            Node *right = ReadSubTree(buf, tree);

            sscanf(buf, " %c%n", &ch, &offset);
            buf += offset;

            if(ch != ')')
            {
                LOG("Invalid data.\n");
                NodeDtor(left );
                NodeDtor(right);
                return NULL;
            }

            tree->size++;
            return NodeCtor(data, type, left, right);
        }
        default:
            buf--;
            return NULL;
    }
}

static size_t FileSize(const char *file_name)
{
    struct stat file_info = {};
    stat(file_name, &file_info);
    return (size_t)file_info.st_size;
}

Tree ReadTree(const char *file_name, VariablesTable *table)
{
    FILE *source  = fopen(file_name, "rb");
    size_t file_size = FileSize(file_name);

    char *buffer = (char *)calloc(file_size + 1, sizeof(char));

    fread(buffer, file_size, sizeof(char), source);
    fclose(source);

    Tree tree  = {};
    tree.table = table;

    tree.root = ReadSubTree(buffer, &tree);

    free(buffer);

    return tree;
}


static Node *SubTreeSearchParent(Node *const node, Node *const search_node)
{
    if(!node) return NULL;
    else if(node->left  == search_node ||
            node->right == search_node) return node;

    Node *find  = SubTreeSearchParent(node->left , search_node);
    return (find ? find : SubTreeSearchParent(node->right, search_node));
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
    if(!root) return EXIT_SUCCESS;

    if(root->left)
    {
        SubTreeDtor(tree, root->left);
        root->left  = NULL;
    }
    if(root->right)
    {
        SubTreeDtor(tree, root->right);
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


static void NodeDataDump(FILE *dump_file, Node *const node)
{
    switch(node->type)
    {
        case VAL:
        {
            fprintf(dump_file, "%lg", node->data.num);
            return;
        }
        case VAR:
        {
            fprintf(dump_file, "%s", node->data.var);
            return;
        }
        case OP:
#define DEF_OP(e_name, e_code, dump, ...) case e_name: {fprintf(dump_file, dump);\
                                          return;}
            switch(node->data.op)
            {
                #include "../include/Operators.h"
                default:
                {
                    fprintf(dump_file, "<unknown operator>");
                    return;
                }
            }
#undef DEF_OP
        case UND: //fall through
        default:
        {
            fprintf(dump_file, "<unknown type>");
            return;
        }
    }
}

static bool OpCmp(const Operator parent_op, const Operator node_op, const bool is_l_child)
{
#define DEF_OP(e_name, e_code, dump, tex, eval, diff, simp, op_cmp) case e_name: {op_cmp;}
    switch(parent_op)
    {
        #include "../include/Operators.h"
        default: return true;
    }
#undef DEF_OP
}

static void SubTreeTextDump(Node *const node, Node *const parent = NULL)
{
    if(!node) return;

    bool brackets = false;
    if(parent && parent->type == OP)
    {
        if(IS_FUNC(parent->data.op) ||
          ((node->type == OP) && OpCmp(parent->data.op, node->data.op, parent->left == node)))
        {
            brackets = true;
        }
    }

    if(brackets) LOG("(");
    SubTreeTextDump(node->left, node);
    NodeDataDump(LOG_FILE, node);
    SubTreeTextDump(node->right, node);
    if(brackets) LOG(")");
}

static void TreeTextDump(Tree *const tree)
{
    LOG("TREE[%p]:\n", tree);

    if(!tree) return;

    LOG("\troot: %p; \n"
        "\tsize: %zu;\n", tree->root, tree->size);

    if(tree->table)
    {
        LOG("\tVariables:\n");
        for(size_t i = 0; i < tree->table->size; i++)
        {
            LOG("\t\t\'%s\' =  %lg;\n", tree->table->vars[i].name, tree->table->vars[i].val);
        }
    }

    SubTreeTextDump(tree->root);
    LOG("\n\n");
}


static void DotNodeCtor(Node *const node, FILE *dot_file)
{
    fprintf(dot_file, "node%p[label = \"{<data>", node);
    NodeDataDump(dot_file, node);

    fprintf(dot_file, "| {<left> left | <right> right}}\"; fillcolor = ");

    switch(node->type)
    {
        case VAL:
        {
            fprintf(dot_file , "\"coral\"];");
            break;
        }
        case OP:
        {
            fprintf(dot_file , "\"orange\"];");
            break;
        }
        case VAR:
        {
            fprintf(dot_file , "\"bisque\"];");
            break;
        }
        case UND: //fall through
        default:
        {
            fprintf(dot_file , "\"red\"];");
        }
    }
}

static void DotSubTreeCtor(Node *const node, Node *const node_next, const char *const direction, FILE *dot_file)
{
    if(!node_next) return;

    DotNodeCtor(node_next, dot_file);

    fprintf(dot_file, "node%p:<%s>:s -> node%p:<data>:n;\n", node, direction, node_next);

    DotSubTreeCtor(node_next, node_next->left , "left" , dot_file);
    DotSubTreeCtor(node_next, node_next->right, "right", dot_file);
}

static void DotTreeGeneral(Tree *const tree, FILE *dot_file)
{
    fprintf(dot_file, "digraph\n"
                      "{\n"
                      "bgcolor = \"grey\";\n"
                      "node[shape = \"Mrecord\"; style = \"filled\"; fillcolor = \"#58CD36\"];\n");

    fprintf(dot_file, "{rank = source;");
    fprintf(dot_file, "nodel[label = \"<root> root: %p | <size> size: %zu\"; fillcolor = \"lightblue\"]; ",
                                                                                   tree->root, tree->size);
    DotNodeCtor(tree->root, dot_file);

    fprintf(dot_file, "variables[label = \"%zu", tree->table->size);
    for(size_t i = 0; i < tree->table->size; i++)
    {
        fprintf(dot_file, "|{%s | %lg}", tree->table->vars[i].name, tree->table->vars[i].val);
    }
    fprintf(dot_file, "\"; fillcolor = \"lightblue\"]};\n");
}

static void TreeDot(Tree *const tree, const char *png_file_name)
{
    if(!(tree && tree->root)) return;

    FILE *dot_file = fopen("tree.dot", "wb");

    DotTreeGeneral(tree, dot_file);
    DotSubTreeCtor(tree->root, tree->root->left , "left" , dot_file);
    DotSubTreeCtor(tree->root, tree->root->right, "right", dot_file);

    fprintf(dot_file, "}\n");

    fclose(dot_file);

    char sys_cmd[BUF_SIZE] = {};
    snprintf(sys_cmd, BUF_SIZE - 1, "dot tree.dot -T png -o %s", png_file_name);
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

    TreeTextDump(tree);

    char file_name[BUF_SIZE] = {};

    sprintf(file_name, "dump_tree/tree_dump%d__%s:%d__.png", num, func, line);
    TreeDot(tree, file_name);

    num++;
}

#ifdef PROTECT
static void TreeValidation(Tree *const tree, Node *const node, size_t *counter)
{
    if(!(node) || (*counter) > tree->size) return;

    if(node->type == VAR)
    {
        ASSERT(node->data.var, return);

        bool in_table = false;
        for(size_t i = 0; i < tree->table->size; i++)
        {
            if(strcmp(tree->table->vars[i].name, node->data.var) == 0)
            {
                in_table = true;
                break;
            }
        }
        ASSERT(in_table, return);
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