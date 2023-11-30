#include <math.h>
#include <time.h>
#include <string.h>

#include "../include/treeMath.h"
#include "../include/treeIO.h"
#include "../include/MathDSL.h"


static double FastPow(double a, long long power);
static Variable *VariablesParsing(Tree *const tree, const char *const var);
static double SubTreeCalculate(Tree *const tree, Node *const node);

static Node *SubTreeDerivative(Node *node, const char *const var, FILE *tex);

static bool SubTreeSearchVar(Node *const node, VariablesTable *table);
static void SubTreeSimplify(Tree *tree, Node *node, FILE *file, bool *changes);

static void PrintRandMessage(FILE *file);

static const char *Phrases[] = {"Очевидно, что:",
                                "Несложно заметить, что:",
                                "Следующий переход подробно описан в многотомных трудах Кантора:",
                                "Это невероятно, но:",
                                "Если этот переход не очевиден, то лучше иди потрогай траву:",
                                "Надеюсь, вас не шокирует, что:",
                                "Следующий переход из серии \'коротко, просто, но не понятно\':",

                                "Первое правило Бойцовского клуба: никому не рассказывать о Бойцовском клубе.\\\\\n"
                                "Второе правило Бойцовского клуба: никому не рассказывать о Бойцовском клубе.\\\\\n"
                                "Третье правило Бойцовского клуба: боец крикнул — «хватит», выдохся, отрубился — бой окончен.\\\\\n"
                                "Четвертое: дерутся только двое.\\\\\n"
                                "Пятое: по поединку за раз.\\\\\n"
                                "Шестое правило: снять обувь, рубашки.\\\\\n"
                                "Седьмое правило Бойцовского клуба: поединок во времени не ограничен.\\\\\n"
                                "Восьмое и последнее правило: если ты в клубе впервые — принимаешь бой."};

static void PrintRandMessage(FILE *file)
{
    fprintf(file, "%s\n", Phrases[rand() % (int)(sizeof(Phrases) / sizeof(char *))]);
}


void FillVariables(VariablesTable *table)
{
    printf("Set the variables:\n");
    double temp = 0;
    for(size_t i = N_CONSTANTS; i < table->size; i++)
    {
        printf("\t%s = ", table->vars[i].name);
        scanf("%lg", &temp);
        if(isfinite(temp))
        {
            table->vars[i].val = temp;
        }
    }
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


static double FastPow(double a, long long power)
{
    if(power == 1) return a;

    if(power % 2 == 0)
    {
        return FastPow(a * a, power / 2);
    }
    else
    {
        return a * FastPow(a, power - 1);
    }
}
#define DEF_OP(e_name, e_code, dump, tex, plot, eval, ...) case e_name: eval
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

            switch(node->data.op)
            {
                #include "../include/Operators.h"
                default:
                {
                    LOG("Unknown operator.\n");
                    return NAN;
                }
            }
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
#undef DEF_OP

double TreeCalculate(Tree *const tree)
{
    TREE_VERIFICATION(tree, NAN);

    double result = SubTreeCalculate(tree, tree->root);
    ASSERT(isfinite(result), return NAN);

    return result;
}


#define DEF_OP(e_name, e_code, dump, tex, plot, eval, diff, simp, ...) case e_name: diff
static Node *SubTreeDerivative(Node *node, const char *const var, FILE *file)
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
        }
        case UND: //fall through
        default:
        {
            LOG("Unknown node type.\n");
            ans =  NULL;
        }
    }

    if(file)
    {
        PrintRandMessage(file);

        TexExprBegin(file);

        fputc('(', file);
        SubTreeTex(node, file);
        fprintf(file, ")\' = ");
        SubTreeTex(ans, file);

        TexExprEnd(file);
    }

    return ans;
}
#undef DEF_OP

Tree Derivative(Tree *const tree, const char *const var, FILE *file)
{
    TREE_VERIFICATION(tree, {});

    Tree derivative = {NULL, tree->table, 1};

    if(file)
    {
        fprintf(file, "\\section{Дифференцирование}\n");
        fprintf(file, "Продифференцируем выражение:\n");
        TreeTex(tree, file);
    }

    if(!VariablesParsing(tree, var))
    {
        derivative.root = NodeCtor({}, VAL);
    }
    else
    {

        derivative.root = SubTreeDerivative(tree->root, var, file);
        derivative.size = SubTreeSize(derivative.root);

    }

    if(file)
    {
        fprintf(file, "Значит:\n");

        TexExprBegin(file);

        fputc('(', file);
        SubTreeTex(tree->root, file);
        fprintf(file, ")\' = ");
        SubTreeTex(derivative.root, file);

        TexExprEnd(file);
    }

    return derivative;
}


Tree TaylorSeries(Tree *expr, const char *var_name, const double point, const size_t n)
{
    TREE_VERIFICATION(expr, {});

    Tree Taylor = {__VAL(TreeCalculate(expr)), expr->table, 1};

    Variable *var = VariablesParsing(expr, var_name);
    if(!var) return Taylor;

    double val_prev = var->val;
    var->val        = point;

    Tree temp      = {};
    Tree tempDiffn = {SubTreeCopy(expr->root), expr->table, expr->size};

    const size_t SizeDiff = 8;
    size_t factorial      = 1;

    for(size_t power = 1; power <= n; power++)
    {
        factorial *= power;
        temp = Derivative(&tempDiffn, var_name);

        Taylor.root = __ADD(Taylor.root, __MUL(__VAL(TreeCalculate(&temp) / (double)factorial),
                                               __POW(__SUB(__VAR(var->name), __VAL(var->val)), __VAL((double)power))));
        Taylor.size += SizeDiff;

        TreeDtor(&tempDiffn, tempDiffn.root);
        tempDiffn = temp;
    }
    TreeDtor(&temp, temp.root);

    var->val = val_prev;

    return Taylor;
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
        case OP:
        {
            bool find  = SubTreeSearchVar(node->left, table);
            return (find ? find : SubTreeSearchVar(node->right, table));
        }
        case VAL:
        {
            break;
        }
        case UND: //fall through
        default:
        {
            LOG("Unknown node type.\n");
        }
    }
    return false;
}

#define DEF_OP(e_name, e_code, dump, tex, plot, eval, diff, simp, op_cmp) case e_name: simp
static void SubTreeSimplify(Tree *tree, Node *node, FILE *file, bool *changes)
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
                switch(node->data.op)
                {
                    #include "../include/Operators.h"
                    default: LOG("Unknown operator.\n"); return;
                }
            }

            if(copy)
            {
                *changes = true;
                if(file)
                {
                    PrintRandMessage(file);

                    TexExprBegin(file);

                    SubTreeTex(node, file);
                    fprintf(file, " = ");
                    SubTreeTex(copy, file);

                    TexExprEnd(file);
                }

                TreeDtor(tree, node->left);
                TreeDtor(tree, node->right);

                tree->size += SubTreeSize(copy) - 1;

                *node = *copy;
                free(copy);

                SubTreeSimplify(tree, node, file, changes);
            }
            else
            {
                SubTreeSimplify(tree, node->left, file, changes);
                SubTreeSimplify(tree, node->right, file, changes);
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
#undef DEF_OP


int TreeSimplify(Tree *tree, FILE *file)
{
    TREE_VERIFICATION(tree, EXIT_FAILURE);

    if(file)
    {
        fprintf(file, "\\section{Упрощение}\n");
        fprintf(file, "Упростим выражение:\n");
        TreeTex(tree, file);
    }

    bool changes = false;
    while(true)
    {
        changes = false;
        SubTreeSimplify(tree, tree->root, file, &changes);
        if(!changes) break;
    }

    if(file)
    {
        fprintf(file, "Таким образом, исходное выражение эквивалентно:\n");
        TreeTex(tree, file);
    }

    return EXIT_SUCCESS;
}