#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../include/tree.h"

static NodeType VariablesProcessing(Tree *tree, char *var_name, data_t *data)
{
    bool exists = false;
    for(size_t i = 0; i < tree->table->size; i++)
    {
        if(strcmp(tree->table->names[i].name, var_name) == 0)
        {
            data->var = tree->table->names[i].name;
            exists    = true;

            break;
        }
    }

    if(!exists)
    {
        ASSERT(tree->table->size < MAX_NAMES, return UND);

        tree->table->names[tree->table->size].name = strdup(var_name);
        ASSERT(tree->table->names[tree->table->size].name, return UND);

        data->var = tree->table->names[tree->table->size++].name;
    }

    return VAR;
}

static NodeType FunctionsProcessing(const char *func_name, data_t *data)
{
    if(strcmp(func_name, "sin") == 0)
        data->func = SIN;
    else if(strcmp(func_name, "cos") == 0)
        data->func = COS;
    else if(strcmp(func_name, "tg") == 0)
        data->func = TG;
    else if(strcmp(func_name, "ctg") == 0)
        data->func = CTG;
    else if(strcmp(func_name, "ln") == 0)
        data->func = LN;
    else
        return UND;

    return FUNC;
}

static NodeType ReadData(char **buf, Tree *tree, data_t *data)
{
    NodeType ans = VAL;
    int offset   = 0;

    int scaned = 0;
    scaned = sscanf(*buf, " %lg%n", &data->num, &offset);
    if(scaned == EOF) return UND;
    else if(!scaned)
    {
        ans = OP;

        char ch = 0;
        scaned = sscanf(*buf, " %c%n", &ch, &offset);
        if(scaned != 1) return UND;

        switch(ch)
        {
            case '(':
                return UND;
            case ')':
                return UND;
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
            case '^':
                data->op = POW;
                break;
            default:
            {
                char name_buf[BUF_SIZE] = {};

                char fmt[BUF_SIZE] = {};
                sprintf(fmt, " %%%zu[^() \n\r\t\v\f]%%n %%c", BUF_SIZE - 1);

                sscanf(*buf, fmt, name_buf, &offset, &ch);
                switch(ch)
                {
                    case '(':
                        ans = FunctionsProcessing(name_buf, data);
                        break;
                    case ')':
                        ans = VariablesProcessing(tree, name_buf, data);
                        break;
                    default:
                        return UND;
                }
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
        {
            buf--;
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

Tree ReadTree(const char *file_name, NamesTable *table)
{
    ASSERT(file_name, return {});

    FILE *source = fopen(file_name, "rb");
    if(!source)
    {
        LOG("No such file: \"%s\"", file_name);
        return {};
    }

    size_t file_size = FileSize(file_name);

    char *buffer = (char *)calloc(file_size + 1, sizeof(char));
    ASSERT(buffer, fclose(source); return {});

    fread(buffer, file_size, sizeof(char), source);
    fclose(source);

    Tree tree  = {};
    tree.table = table;

    tree.root = ReadSubTree(buffer, &tree);

    free(buffer);

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
    ASSERT(root == tree->root || (TreeSearchParent(tree, root) != NULL), return EXIT_FAILURE);

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


Node *SubTreeCopy(Node *const node, size_t *counter)
{
    if(!node) return NULL;
    else if(counter) (*counter)++;

    return NodeCtor(node->data, node->type, SubTreeCopy(node->left, counter), SubTreeCopy(node->right, counter));
}


static Node *SubTreeSearchParent(Node *const node, Node *const search_node)
{
    if(!node) return NULL;
    else if(node->left  == search_node ||
            node->right == search_node) return node;

    Node *find  = SubTreeSearchParent(node->left , search_node);

    return (find ? find : SubTreeSearchParent(node->right, search_node));
}

Node *TreeSearchParent(Tree *const tree, Node *const search_node)
{
    TREE_VERIFICATION(tree, NULL);
    ASSERT(search_node, return NULL);

    return SubTreeSearchParent(tree->root, search_node);
}


static void NodeDataDump(FILE *dump_file, Node *const node)
{
    switch(node->type)
    {
        case OP:
        {
            fputc(' ', dump_file);
            switch(node->data.op)
            {
                case ADD:
                    fputc('+', dump_file);
                    break;
                case SUB:
                    fputc('-', dump_file);
                    break;
                case MUL:
                    fputc('*', dump_file);
                    break;
                case DIV:
                    fputc('/', dump_file);
                    break;
                case POW:
                    fputc('^', dump_file);
                    break;
                default:
                    fputc('?', dump_file);
                    break;
            }
            fputc(' ', dump_file);

            return;
        }
        case FUNC:
        {
            switch(node->data.func)
            {
                case SIN:
                    fprintf(dump_file, "sin");
                    break;
                case COS:
                    fprintf(dump_file, "cos");
                    break;
                case TG:
                    fprintf(dump_file, "tg");
                    break;
                case CTG:
                    fprintf(dump_file, "ctg");
                    break;
                case LN:
                    fprintf(dump_file, "ln");
                    break;
                default:
                    fprintf(dump_file, "func");
                    break;
            }
            return;
        }
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
        case UND: //fall through
        default:
        {
            fprintf(dump_file, "<unknown type>");
            return;
        }
    }
}

static int OpCmp(const Operator parent_t, const Operator node_t, const NodePos node_pos)
{
    if(parent_t == ADD)
    {
        return -1;
    }
    else if(parent_t == MUL)
    {
        if(node_t == MUL || node_t == DIV)
        {
            return -1;
        }

        return 1;
    }
    else if(parent_t == DIV)
    {
       if(node_pos == LEFT && (node_t == MUL || node_t == DIV))
       {
            return -1;
       }

       return 1;
    }
    else if(parent_t == SUB)
    {
        if(node_pos == LEFT || node_t == MUL || node_t == DIV)
        {
            return -1;
        }

        return 1;
    }

    return 1;
}

static void SubTreeTextDump(Node *const node, const NodePos n_pos, Node *const parent = NULL)
{
    if(!node) return;

    int op_cmp = 0;
    if(parent && node->type == OP && parent->type == OP)
    {
        op_cmp = OpCmp(parent->data.op, node->data.op, n_pos);
    }
    else if(parent && parent->type == FUNC)
    {
        op_cmp = 1;
    }
    else op_cmp = -1;

    if(op_cmp > 0) LOG("(");

    SubTreeTextDump(node->left, LEFT, node);

    NodeDataDump(LOG_FILE, node);

    SubTreeTextDump(node->right, RIGHT, node);

    if(op_cmp > 0) LOG(")");
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
            LOG("\t\t\'%s\' =  %lg;\n", tree->table->names[i].name, tree->table->names[i].val);
        }
    }

    SubTreeTextDump(tree->root, AUTO);
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
            fprintf(dot_file , "\"orchid\"];");
            break;
        case OP:
            fprintf(dot_file , "\"green\"];");
            break;
        case VAR:
            fprintf(dot_file , "\"blue\"];");
            break;
        case FUNC:
            fprintf(dot_file , "\"orange\"];");
            break;
        case UND: //fall through
        default:
            fprintf(dot_file , "\"red\"];");
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
        fprintf(dot_file, "|{%s | %lg}", tree->table->names[i].name, tree->table->names[i].val);
    }
    fprintf(dot_file, "\"; fillcolor = \"lightblue\"]};\n");
}

void TreeDot(Tree *const tree, const char *png_file_name)
{
    ASSERT(png_file_name, return);

    if(!(tree && tree->root)) return;

    FILE *dot_file = fopen("tree.dot", "wb");
    ASSERT(dot_file, return);

    setbuf(dot_file, NULL);

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
    if(!(node) || (*counter) >= tree->size) return;

    if(node->type == VAR)
    {
        ASSERT(node->data.var, return);

        bool in_table = false;
        for(size_t i = 0; i < tree->table->size; i++)
        {
            if(strcmp(tree->table->names[i].name, node->data.var) == 0)
            {
                in_table = true;
                break;
            }
        }
        ASSERT(in_table, return);
    }

    if(node->type == OP)
    {
        ASSERT(node->left && node->right, return);
    }
    else if(node->type == FUNC)
    {
        ASSERT(!node->left && node->right, return);
    }
    else
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