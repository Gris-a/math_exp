#include <math.h>
#include <time.h>
#include <string.h>

#include "../include/treeMath.h"
#include "../include/MathDSL.h"

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
#define DEF_OP(e_name, e_code, dump, tex, ...) case e_name: {fprintf(tex_file, tex);\
                                               return;}
            switch(node->data.op)
            {
                #include "../include/Operators.h"
                default:
                {
                    fprintf(tex_file, "<unknown operator>");
                    return;
                }
            }
#undef DEF_OP
        case UND: //fall through
        default:
        {
            fprintf(tex_file, "<unknown type>");
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

static void SubTreeTex(Node *const node, FILE *tex_file, Node *const parent = NULL)
{
    if(!node) return;

    bool brackets = false;
    if(parent)
    {
        if(IS_FUNC(parent->data.op) ||
          ((node->type == OP) && OpCmp(parent->data.op, node->data.op, parent->left == node)))
        {
            brackets = true;
        }
    }
    bool c_brackets = (parent && (parent->data.op == DIV || (parent->data.op == POW && parent->right == node)));

    if(c_brackets) fprintf(tex_file, "{");
    if(brackets) fprintf(tex_file, "(");
    if(node->type == OP)
    {
        if(IS_PREFIX(node->data.op))
        {
            NodeDataTex(tex_file, node);
            SubTreeTex(node->left, tex_file, node);
        }
        else
        {
            SubTreeTex(node->left, tex_file, node);
            NodeDataTex(tex_file, node);
        }
        SubTreeTex(node->right, tex_file, node);
    }
    else
    {
        NodeDataTex(tex_file, node);
    }

    if(brackets) fprintf(tex_file, ")");
    if(c_brackets) fprintf(tex_file, "}");
}

int TreeTex(Tree *const tree, FILE *tex_file)
{
    TREE_VERIFICATION(tree, EXIT_FAILURE);

    ASSERT(tex_file, return EXIT_FAILURE);

    fprintf(tex_file, "\n$$");
    SubTreeTex(tree->root, tex_file);
    fprintf(tex_file, "$$\n");

    return EXIT_SUCCESS;
}





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
        case VAL:
        {
            return node->data.num;
        }
        case OP:
        {
            double calc_left  = SubTreeCalculate(tree, node->left);
            double calc_right = SubTreeCalculate(tree, node->right);

#define DEF_OP(e_name, e_code, dump, tex, eval, ...) case e_name: {eval;}
            switch(node->data.op)
            {
                #include "../include/Operators.h"
                default:
                {
                    LOG("Unknown operator.\n");
                    return NAN;
                }
            }
#undef DEF_OP
        }
        case VAR:
        {
            Variable *var = VariablesParsing(tree, node->data.var);
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





static Node *SubTreeCopy(Node *sub_tree)
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

static size_t SubTreeSize(Node *sub_tree)
{
    size_t size = 0;
    SubTreeSizeCounter(sub_tree, &size);

    return size;
}





static Node *SubTreeDerivative(Node *node, const char *const var, FILE *tex)
{
    ASSERT(node, return NULL);

    Node *ans = NULL;
    switch(node->type)
    {
        case VAL:
        {
            ans =  __VAL(0);
            break;
        }
        case VAR:
        {
            ans = __VAL(strcmp(node->data.var, var) == 0);
            break;
        }
        case OP:
        {
#define DEF_OP(e_name, e_code, dump, tex, eval, diff, simp, ...) case e_name: {diff; break;}
            switch(node->data.op)
            {
                #include "../include/Operators.h"
                default:
                {
                    LOG("Unknown operator.\n");
                    ans =  NULL;
                }
            }
            break;
#undef DEF_OP
        }
        case UND: //fall through
        default:
        {
            LOG("Unknown node type.\n");
            ans =  NULL;
        }
    }

    fprintf(tex, "$$(");
    SubTreeTex(node, tex);
    fprintf(tex, ")` = ");
    SubTreeTex(ans, tex);
    fprintf(tex, "$$\n");

    return ans;
}

Tree Derivative(Tree *const tree, const char *const var)
{
    TREE_VERIFICATION(tree, {});

    Tree derivative = {NULL, tree->table, 1};
    FILE *tex = fopen("derivative.tex", "wb");

    if(!VariablesParsing(tree, var))
    {
        derivative.root = NodeCtor({}, VAL);
    }
    else
    {
        derivative.root = SubTreeDerivative(tree->root, var, tex);
        derivative.size = SubTreeSize(derivative.root);
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
        {
            return false;
        }
        case OP:
        {
            bool find  = SubTreeSearchVar(node->left, table);
            return (find ? find : SubTreeSearchVar(node->right, table));
        }
        case UND:
        default:
        {
            LOG("Unknown node type.\n");
            return false;
        }
    }
}

static void SubTreeSimplify(Tree *tree, Node *node, FILE *file)
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
#define DEF_OP(e_name, e_code, dump, tex, eval, diff, simp, op_cmp) case e_name: {simp; break;}
                switch(node->data.op)
                {
                    #include "../include/Operators.h"
                    default: LOG("Unknown operator.\n"); return;
                }
#undef DEF_OP
            }

            if(copy)
            {
                fprintf(file, "$$");
                SubTreeTex(node, file);
                fprintf(file, " = ");
                SubTreeTex(copy, file);
                fprintf(file, "$$\n");

                TreeDtor(tree, node->left);
                TreeDtor(tree, node->right);

                tree->size += SubTreeSize(copy) - 1;

                *node = *copy;
                free(copy);

                SubTreeSimplify(tree, node, file);
            }
            else
            {
                SubTreeSimplify(tree, node->left, file);
                SubTreeSimplify(tree, node->right, file);
            }

            return;
        }
        case UND: //fall through
        default:
        {
            LOG("Unknown node type.\n");
            return;
        }
    }
}

int TreeSimplify(Tree *tree, size_t n_iter)
{
    TREE_VERIFICATION(tree, EXIT_FAILURE);

    FILE *file = fopen("simp.tex", "wb");

    for(size_t i = 0; i < n_iter; i++)
    {
        SubTreeSimplify(tree, tree->root, file);
    }

    return EXIT_SUCCESS;
}