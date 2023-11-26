#include <math.h>
#include <time.h>
#include <string.h>

#include "../include/treeMath.h"
#include "../include/MathDSL.h"

static Variable *VariablesParsing(Tree *const tree, const char *const var)
{
    for(size_t i = 0; i < tree->table->size; i++)
    {
        if(strcmp(tree->table->vars[i].name, var) == 0)
        {
            return tree->table->vars + i;
        }
    }

    return NULL;
}

static double SubTreeCalculate(Tree *const tree, Node *const node)
{
    if(!node) return NAN;

    switch(node->type)
    {
        case VAL: return node->data.num;
        case OP:
        {
            double calc_left  = SubTreeCalculate(tree, node->left);
            double calc_right = SubTreeCalculate(tree, node->right);

#define DEF_OP(enum, literal, eval, dif, smpl) case enum: eval;

            switch(node->data.op)
            {
                #include "../include/Operators.h"
                default: LOG("Unknown operator.\n"); return NAN;
            }

#undef DEF_OP
        }
        case VAR:
        {
            Variable *var = VariablesParsing(tree, node->data.var);
            return var->val;
        }
        case UND: //fall through
        default:
            LOG("Unknown node type.\n");
            return NAN;
    }

    return NAN;
}

double TreeCalculate(Tree *const tree)
{
    TREE_VERIFICATION(tree, NAN);

    return SubTreeCalculate(tree, tree->root);
}


static Node *SubTreeCopy(Node *sub_tree)
{
    if(!sub_tree) return NULL;
    return NodeCtor(sub_tree->data, sub_tree->type, SubTreeCopy(sub_tree->left),
                                                    SubTreeCopy(sub_tree->right));
}

static void SubTreeSize(Node *sub_tree, size_t *size)
{
    if(!sub_tree) return;
    (*size)++;

    SubTreeSize(sub_tree->left , size);
    SubTreeSize(sub_tree->right, size);
}


static Node *SubTreeDerivative(Node *node, const char *const var)
{
    ASSERT(node, return NULL);

    switch(node->type)
    {
        case VAL:
            return __VAL(0);
        case VAR:
            return __VAL(!strcmp(node->data.var, var));
        case OP:
        {
#define DEF_OP(enum, literal, eval, dif, smpl) case enum: dif
            switch(node->data.op)
            {
                #include "../include/Operators.h"
                default: LOG("Unknown operator.\n"); return NULL;
            }
#undef DEF_OP
        }
        case UND: //fall through
        default: LOG("Unknown node type.\n"); return NULL;
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
        derivative.root = SubTreeDerivative(tree->root, var);
        SubTreeSize(derivative.root, &derivative.size);
    }

    return derivative;
}


static bool SubTreeSearchVar(Node *const node, VariablesTable *table)
{
    if(!node) return false;

    switch(node->type)
    {
        case VAR:
        {
            bool is_constant = false;
            for(size_t i = 0; i < N_CONSTANTS; i++)
            {
                if(node->data.var == table->vars[i].name)
                {
                    is_constant = true;
                    break;
                }
            }
            return (!is_constant);
        }
        case VAL:
            return false;
        case OP:
        {
            bool find  = SubTreeSearchVar(node->left, table);
            return (find ? find : SubTreeSearchVar(node->right, table));
        }
        case UND:
        default:
            return false;
    }
}

static void SubTreeSimplify(Tree *tree, Node *node)
{
    if(!node) return;

    switch(node->type)
    {
        case VAL: return;
        case VAR: return;
        case OP:
        {
            Node *copy = NULL;
            if(!HAS_VAR(node))
            {
                copy = __VAL(CALC(node));
            }
            else
            {
#define DEF_OP(enum_name, literal, evaluate, differenciate, simplify) case enum: simplify; break;

                switch(node->data.op)
                {
                    #include "../include/Operators.h"
                    default: LOG("Unknown operator.\n"); return;
                }

#undef DEF_OP
            }

            if(copy)
            {
                TreeDtor(tree, node->left);
                TreeDtor(tree, node->right);

                size_t copy_size = 0;
                SubTreeSize(copy, &copy_size);

                *node = *copy;
                free(copy);

                tree->size += copy_size - 1;

                SubTreeSimplify(tree, node);
            }
            else
            {
                SubTreeSimplify(tree, node->left);
                SubTreeSimplify(tree, node->right);
            }

            return;
        }
        case UND: //fall through
        default: LOG("Unknown node type.\n"); return;
    }
}

int TreeSimplify(Tree *tree, size_t n_iter)
{
    TREE_VERIFICATION(tree, EXIT_FAILURE);

    for(size_t i = 0; i < n_iter; i++)
    {
        SubTreeSimplify(tree, tree->root);
    }

    return EXIT_SUCCESS;
}