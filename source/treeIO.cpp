#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdarg.h>

#include "../include/treeIO.h"

static NodeType VariablesProcessing(Tree *tree, data_t *data, char *const var_name);
static NodeType OperatorsProcessing(data_t *data, const char *op_name);
static NodeType ReadData(char **buf, Tree *tree, data_t *data);
static Node *ReadSubTree(char *const buffer, Tree *tree);
static size_t FileSize(const char *file_name);

static bool OpCmp(const Operator parent_op, const Operator node_op, const bool is_l_child);

static bool NeedBrackets     (Node *const parent, Node *const node);
static bool NeedCurlyBrackets(Node *const parent, Node *const node);

static void NodeDataDump(FILE *dump_file, Node *const node);
static void NodeDataTex (FILE *tex_file , Node *const node);
static void NodeDataPlot(FILE *tex_file , Node *const node);

static void SubTreePrint(FILE *file, void (*Print)(FILE *const, Node *const), Node *const node, Node *const parent = NULL);

static void VariablesDump(VariablesTable *const table, FILE *dump_file);

static void TexPrefix(Node *const node, FILE *tex_file);
static void TexInfix(Node *const node, FILE *tex_file);

static void DotNodeCtor(Node *const node, FILE *dot_file);
static void DotSubTreeCtor(Node *const node, Node *const node_next, const char *const direction, FILE *dot_file);
static void DotTreeGeneral(Tree *const tree, FILE *dot_file);

static void PlotGeneral(FILE *plot_script, const double lx_bound, const double rx_bound, const double ly_bound, const double ry_bound, char *plot_name);



static NodeType VariablesProcessing(Tree *tree, data_t *data, char *const var_name)
{
    Variable *var = VariablesParsing(tree->table, var_name);

    if(!var)
    {
        if(tree->table->size >= MAX_VARIABLES) return UND;

        tree->table->vars[tree->table->size].name = strdup(var_name);
        data->var = tree->table->vars[tree->table->size++].name;
    }
    else
    {
        data->var = var->name;
    }

    return VAR;
}

#define DEF_OP(enum_name, e_code, literal, ...) if(strcmp(op_name, literal) == 0)\
                                                {data->op = enum_name;} else
static NodeType OperatorsProcessing(data_t *data, const char *op_name)
{
    #include "../include/Operators.h"
    /*else*/ return UND;

    return OP;
}
#undef DEF_OP

static NodeType ReadData(char **buf, Tree *tree, data_t *data)
{
    const size_t BUF_SIZE = 1000;

    NodeType ans = VAL;
    int offset   = 0;

    int scaned = 0;
    scaned = sscanf(*buf, " %lg %n", &data->num, &offset);
    if(!scaned)
    {
        ans    = OP;
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

Tree ReadTree(const char *file_name, VariablesTable *table)
{
    ASSERT(file_name && table, return {});

    FILE *source  = fopen(file_name, "rb");
    ASSERT(source, return {});

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





#define DEF_OP(enum_name, e_code, dump, tex, plot, eval, diff, simp, op_compare) case enum_name: op_compare;
static bool OpCmp(const Operator parent_op, const Operator node_op, const bool is_l_child)
{
    switch(parent_op)
    {
        #include "../include/Operators.h"
        default: return true;
    }
}
#undef DEF_OP

static bool NeedBrackets(Node *const parent, Node *const node)
{
    return parent && (IS_FUNC(parent->data.op) ||
                      ((node->type == OP) && OpCmp(parent->data.op, node->data.op, (parent->left == node))));
}

static bool NeedCurlyBrackets(Node *const parent, Node *const node)
{
    return (parent && ((parent->data.op == DIV) || ((parent->data.op == POW) && (parent->right == node))));
}


#define DEF_OP(enum_name, e_code, dump, ...) case enum_name: {fprintf(dump_file, dump);\
                                          return;}
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
            switch(node->data.op)
            {
                #include "../include/Operators.h"
                default:
                {
                    fprintf(dump_file, "<unknown operator>");
                    return;
                }
            }
        case UND: //fall through
        default:
        {
            fprintf(dump_file, "<unknown type>");
            return;
        }
    }
}
#undef DEF_OP

#define DEF_OP(enum_name, e_code, dump, tex, ...) case enum_name: {fprintf(tex_file, tex);\
                                               return;}
static void NodeDataTex(FILE *tex_file, Node *const node)
{
    switch(node->type)
    {
        case VAL:
        {
            fprintf(tex_file, "%lg", node->data.num);
            return;
        }
        case VAR:
        {
            fprintf(tex_file, "%s", node->data.var);
            return;
        }
        case OP:
            switch(node->data.op)
            {
                #include "../include/Operators.h"
                default:
                {
                    fprintf(tex_file, "<unknown operator>");
                    return;
                }
            }
        case UND: //fall through
        default:
        {
            fprintf(tex_file, "<unknown type>");
            return;
        }
    }
}
#undef DEF_OP

#define DEF_OP(enum_name, e_code, dump, tex, plot, ...) case enum_name: {fprintf(tex_file, plot);\
                                               return;}
static void NodeDataPlot(FILE *tex_file, Node *const node)
{
    switch(node->type)
    {
        case VAL:
        {
            fprintf(tex_file, "%lg", node->data.num);
            return;
        }
        case VAR:
        {
            fprintf(tex_file, "%s", node->data.var);
            return;
        }
        case OP:
            switch(node->data.op)
            {
                #include "../include/Operators.h"
                default:
                {
                    fprintf(tex_file, "<unknown operator>");
                    return;
                }
            }
        case UND: //fall through
        default:
        {
            fprintf(tex_file, "<unknown type>");
            return;
        }
    }
}
#undef DEF_OP


static void SubTreePrint(FILE *file, void (*Print)(FILE *const, Node *const), Node *const node, Node *const parent)
{
    if(!node) return;

    bool brackets = NeedBrackets(parent, node);

    if(brackets) fprintf(file, "(");

    SubTreePrint(file, Print, node->left, node);
    Print(file, node);
    SubTreePrint(file, Print, node->right, node);

    if(brackets) fprintf(file, ")");
}





static void VariablesDump(VariablesTable *const table, FILE *dump_file)
{
    if(table)
    {
        fprintf(dump_file, "\tVariables:\n");
        for(size_t i = 0; i < table->size; i++)
        {
            fprintf(dump_file, "\t\t\'%s\' =  %lg;\n", table->vars[i].name, table->vars[i].val);
        }
    }
}

void TreeTextDump(Tree *const tree, FILE *dump_file)
{
    fprintf(dump_file, "TREE[%p]:\n", tree);

    if(!tree) return;

    fprintf(dump_file, "\troot: %p; \n"
                       "\tsize: %zu;\n", tree->root, tree->size);

    VariablesDump(tree->table, dump_file);
    SubTreePrint(dump_file, NodeDataDump, tree->root);

    fprintf(dump_file, "\n\n");
}





static void TexPrefix(Node *const node, FILE *tex_file)
{
    NodeDataTex(tex_file, node);
    SubTreeTex(node->left, tex_file, node);
    SubTreeTex(node->right, tex_file, node);
}

static void TexInfix(Node *const node, FILE *tex_file)
{
    SubTreeTex(node->left, tex_file, node);
    NodeDataTex(tex_file, node);
    SubTreeTex(node->right, tex_file, node);
}

void SubTreeTex(Node *const node, FILE *tex_file, Node *const parent)
{
    if(!node) return;

    bool brackets = NeedBrackets(parent, node);
    bool curly_br = NeedCurlyBrackets(parent, node);

    if(curly_br) fprintf(tex_file, "{");
    if(brackets) fprintf(tex_file, "(");

    if(node->type == OP)
    {
        if(IS_PREFIX(node->data.op))
        {
            TexPrefix(node, tex_file);
        }
        else
        {
            TexInfix(node, tex_file);
        }
    }
    else
    {
        NodeDataTex(tex_file, node);
    }

    if(brackets) fprintf(tex_file, ")");
    if(curly_br) fprintf(tex_file, "}");
}

void TexExprBegin(FILE *tex_file)
{
    fprintf(tex_file, "\n\\begin{dmath*}\n");
}

void TexExprEnd(FILE *tex_file)
{
    fprintf(tex_file, "\n\\end{dmath*}\n");
}

void TexStart(FILE *tex_file)
{
    fprintf(tex_file, "\\documentclass[12pt]{article}\n"
                      "\\usepackage[T2A]{fontenc}\n"
                      "\\usepackage{amsmath}\n"
                      "\\usepackage{breqn}\n"
                      "\\usepackage{float}\n"
                      "\\usepackage[english, russian]{babel}\n"
                      "\n"
                      "\\begin{document}\n");
}

void TexEnd(FILE *tex_file)
{
    fprintf(tex_file, "\\end{document}\n");
}

void TexImg(FILE *tex_file, const char *path, const char *message)
{
    if(message) fprintf(tex_file, "%s\n", message);
    fprintf(tex_file, "\\begin{figure}[H]\n"
                      "\\includegraphics[scale=0.3]{%s}\n"
                      "\\centering\n"
                      "\\end{figure}\n", path);
}

int TreeTex(Tree *const tree, FILE *tex_file)
{
    TREE_VERIFICATION(tree, EXIT_FAILURE);

    ASSERT(tex_file, return EXIT_FAILURE);

    TexExprBegin(tex_file);

    SubTreeTex(tree->root, tex_file);

    TexExprEnd(tex_file);

    return EXIT_SUCCESS;
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

void TreeDot(Tree *const tree, const char *png_file_name)
{
    if(!(tree && tree->root)) return;

    FILE *dot_file = fopen("tree.dot", "wb");

    DotTreeGeneral(tree, dot_file);
    DotSubTreeCtor(tree->root, tree->root->left , "left" , dot_file);
    DotSubTreeCtor(tree->root, tree->root->right, "right", dot_file);

    fprintf(dot_file, "}\n");

    fclose(dot_file);

    char *sys_cmd = NULL;
    asprintf(&sys_cmd, "dot tree.dot -T png -o %s", png_file_name);
    system(sys_cmd);
    free(sys_cmd);

    remove("tree.dot");
}





static void PlotGeneral(FILE *plot_script, const double x_min, const double x_max, const double y_min, const double y_max, char *png_name)
{
    static int num = 0;

    bool is_name_generated = false;
    if(!png_name)
    {
        asprintf(&png_name, "plot/plot%d.png", num);
        is_name_generated = true;
    }

    fprintf(plot_script, "#! /usr/bin/gnuplot -persist\n"
                         "set xlabel \"X\"\n"
                         "set ylabel \"Y\"\n"
                         "set grid\n"
                         "set terminal png size 1920,1080 lw 3 font \"Times new roman, 30\"\n"
                         "set output '%s'\n"
                         "set xrange[%lf:%lf]\n"
                         "set yrange[%lf:%lf]\n"
                         "plot ", png_name, x_min, x_max, y_min, y_max);

    if(is_name_generated)
    {
        free(png_name);
    }
    else
    {
        num++;
    }
}

int TreePlot(PlotStatus status, ...)
{
    static FILE *plot_script = NULL;
    static char *script_name = NULL;

    va_list args = {};
    va_start(args, status);

    switch(status)
    {
        case START: //arguments: OX and OY bounds, file name of future png
        {
            ASSERT(!plot_script, return EXIT_FAILURE);
            ASSERT(!script_name, return EXIT_FAILURE);

            script_name = tmpnam(NULL);
            plot_script = fopen(script_name, "wb");

            double x_min = va_arg(args, double);
            double x_max = va_arg(args, double);

            double y_min = va_arg(args, double);
            double y_max = va_arg(args, double);

            char *png_name = va_arg(args, char *);

            PlotGeneral(plot_script, x_min, x_max, y_min, y_max, png_name);

            break;
        }
        case PLOT: //arguments: expression tree and title
        {
            ASSERT(plot_script, return EXIT_FAILURE);

            Tree *expr  = va_arg(args, Tree *);
            char *title = va_arg(args, char *);

            TREE_VERIFICATION(expr, EXIT_FAILURE);
            SubTreePrint(plot_script, NodeDataPlot, expr->root);

            if(title) fprintf(plot_script, " title \'%s\'", title);
            fputc(',', plot_script);

            break;
        }
        case END: //arguments: None
        {
            fclose(plot_script);
            plot_script = NULL;

            char *make_exec = NULL;
            asprintf(&make_exec, "chmod +x %s", script_name);

            system(make_exec);
            system(script_name);
            system("clear");

            remove(script_name);
            script_name = NULL;

            free(make_exec);

            break;
        }
        default:
        {
            return EXIT_FAILURE;
        }
    }

    va_end(args);

    return EXIT_SUCCESS;
}