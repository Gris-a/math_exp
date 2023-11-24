#include "math.h"
#include <time.h>
#include <string.h>

#include "../include/diff.h"
//TODO check if var in the tree(for derivative)
static Name *VariablesParsing(Tree *const tree, const char *const var)
{
    for(size_t i = 0; i < tree->table->size; i++)
    {
        if(strcmp(tree->table->names[i].name, var) == 0)
        {
            return tree->table->names + i;
        }
    }

    return NULL;
}

static double SubTreeCalculate(Tree *const tree, Node *const node)
{
    if(!node) return NAN;

    double calc_left  = SubTreeCalculate(tree, node->left );
    double calc_right = SubTreeCalculate(tree, node->right);

    switch(node->type)
    {
        case VAL: return node->data.num;
        case OP:
        {
            switch(node->data.op)
            {
                case ADD:
                    return calc_left + calc_right;
                case SUB:
                    return calc_left - calc_right;
                case MUL:
                    return calc_left * calc_right;
                case POW:
                    return pow(calc_left, calc_right);
                case DIV:
                    return calc_left / calc_right;
                default:
                    LOG("Unknown operator.\n");
                    return NAN;
            }
        }
        case FUNC:
        {
            switch(node->data.func)
            {
                case SIN:
                    return sin(calc_right);
                case COS:
                    return cos(calc_right);
                case TG:
                    return tan(calc_right);
                case CTG:
                    return 1 / tan(calc_right);
                case LN:
                    return log(calc_right);
                default:
                    return NAN;
            }
        }
        case VAR:
        {
            Name *var = VariablesParsing(tree, node->data.var);
            ASSERT(var, return NAN);

            return var->val;
        }
        case UND: //fall through
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


static void NodeDataTex(FILE *dump_file, Node *const node)
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
                    fprintf(dump_file, "\\cdot");
                    break;
                case DIV:
                    fprintf(dump_file, "}{");
                    break;
                case POW:
                    fprintf(dump_file, "^{");
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

static void SubTreeTex(Node *const node, const NodePos n_pos, FILE *tex_file, Node *const parent = NULL)
{
    if(!node) return;

    int op_cmp = 0;
    if(parent && parent->type == OP &&node->type == OP)
    {
        op_cmp = OpCmp(parent->data.op, node->data.op, n_pos);
    }
    else if(parent && parent->type == FUNC)
    {
        op_cmp = 1;
    }
    else op_cmp = -1;

    if(op_cmp > 0)
    {
        fputc('(', tex_file);
    }
    if(node->type == OP && node->data.op == DIV)
    {
        fprintf(tex_file, "\\frac{");
    }

    SubTreeTex(node->left, LEFT, tex_file, node);
    NodeDataTex(tex_file, node);
    SubTreeTex(node->right, RIGHT, tex_file, node);

    if(node->type == OP && (node->data.op == DIV || node->data.op == POW))
    {
        fputc('}', tex_file);
    }
    if(op_cmp > 0)
    {
        fputc(')', tex_file);
    }
}

int TreeTex(Tree *const tree, const char *const file_name)
{
    TREE_VERIFICATION(tree, EXIT_FAILURE);
    ASSERT(file_name, return EXIT_FAILURE);

    FILE *tex_file = fopen(file_name, "wb");
    ASSERT(tex_file, return EXIT_FAILURE);

    setbuf(tex_file, NULL);

    fprintf(tex_file, "\\documentclass[12pt,a4paper]{extreport}\n"
                       "\\begin{document}\n");

    fprintf(tex_file, "$$");
    SubTreeTex(tree->root, AUTO, tex_file);
    fprintf(tex_file, "$$");

    fprintf(tex_file, "\n\\end{document}\n\n");

    fclose(tex_file);

    return EXIT_SUCCESS;
}


static Node *SubTreeDerivative(Node *node, const char *const var, size_t *counter)
{
    ASSERT(node, return NULL);

    (*counter)++;
    switch(node->type)
    {
        case VAL:
        {
            data_t data = {};
            data.num    = 0;

            return NodeCtor(data, VAL);
        }
        case VAR:
        {
            data_t data = {};
            data.num    = (strcmp(node->data.var, var) == 0) ? 1 : 0;

            return NodeCtor(data, VAL);
        }
        case OP:
        {
            Node *left  = NULL;
            Node *right = NULL;

            switch(node->data.op)
            {
                case ADD:
                {
                    left  = SubTreeDerivative(SubTreeCopy(node->left , counter), var, counter);
                    right = SubTreeDerivative(SubTreeCopy(node->right, counter), var, counter);

                    return NodeCtor({ADD}, OP, left, right);
                }
                case SUB:
                {
                    left  = SubTreeDerivative(SubTreeCopy(node->left , counter), var, counter);
                    right = SubTreeDerivative(SubTreeCopy(node->right, counter), var, counter);

                    return NodeCtor({SUB}, OP, left, right);
                }
                case MUL:
                {
                    left  = NodeCtor({MUL}, OP, SubTreeCopy(node->right, counter), SubTreeDerivative(node->left , var, counter));
                    right = NodeCtor({MUL}, OP, SubTreeCopy(node->left , counter), SubTreeDerivative(node->right, var, counter));

                    return NodeCtor({ADD}, OP, left, right);
                }
                case DIV:
                {
                    left  = NodeCtor({SUB}, OP, NodeCtor({MUL}, OP, SubTreeCopy(node->right, counter), SubTreeDerivative(node->left , var, counter)),
                                                NodeCtor({MUL}, OP, SubTreeCopy(node->left , counter), SubTreeDerivative(node->right, var, counter)));
                    right = NodeCtor({MUL}, OP, SubTreeCopy(node->right, counter), SubTreeCopy(node->right, counter));

                    return NodeCtor({DIV}, OP, left, right);
                }
                default: return NULL;
            }
        }
        case UND:
        default: return NULL;
    }
}

Tree Derivative(Tree *const tree, const char *const var)
{
    TREE_VERIFICATION(tree, {});

    Tree derivative = {NULL, tree->table, 0};


    if(!VariablesParsing(tree, var))
    {
        derivative.root = NodeCtor({}, VAL);
    }
    else
    {
        size_t size     = 0;
        derivative.root = SubTreeDerivative(tree->root, var, &size);
        derivative.size = size;
    }

    return derivative;
}