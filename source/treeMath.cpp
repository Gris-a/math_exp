#include <math.h>
#include <time.h>
#include <string.h>

#include "../include/treeMath.h"
#include "../include/treeIO.h"
#include "../include/MathDSL.h"


static double FastPow(double a, long long power);
static double SubTreeCalculate(Tree *const tree, Node *const node);

static void PrintRandMessage(FILE *file, const char *message = NULL);

static void TexDerivEq(FILE *file, Node *const node, Node *const deriv);
static Node *SubTreeDerivative(Node *node, const char *const var, FILE *tex);

static bool SubTreeSearchVar(Node *const node, VariablesTable *table);
static void SubTreeSimplify(Tree *tree, Node *node, FILE *file, bool *changes);


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

static void PrintRandMessage(FILE *file, const char *message)
{
    if(!message)
    {
        fprintf(file, "%s\n", Phrases[rand() % (int)(sizeof(Phrases) / sizeof(char *))]);
    }
    else
    {
        fprintf(file, "%s\n", message);
    }
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


static double FastPow(double base, long long power)
{
    double result = 1;
    while(power > 0)
    {
        if(power % 2 == 1)
        {
            result *= base;
        }

        base  *= base;
        power /= 2;
    }

    return result;
}


#define DEF_OP(enum_name, e_code, dump, tex, plot, evaluate, ...) case enum_name: evaluate
static double SubTreeCalculate(Tree *const tree, Node *const node)
{
    if(!node) return NAN;

    switch(node->type)
    {
        case OP:
        {
            double lvalue = SubTreeCalculate(tree, node->left);
            double rvalue = SubTreeCalculate(tree, node->right);

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
        case VAL:
        {
            return node->data.num;
        }
        case VAR:
        {
            Variable *var = VariablesParsing(tree->table, node->data.var);
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


static void TexDerivEq(FILE *file, Node *const node, Node *const deriv)
{
    TexExprBegin(file);
    fputc('(', file);
    SubTreeTex(node, file);
    fprintf(file, ")\' = ");
    SubTreeTex(deriv, file);
    TexExprEnd(file);
}

#define DEF_OP(enum_name, e_code, dump, tex, plot, eval, differentiate, simp, ...) case enum_name: differentiate
static Node *SubTreeDerivative(Node *const node, const char *const var, FILE *file)
{
    ASSERT(node, return NULL);

    Node *ans = NULL;
    switch(node->type)
    {
        case OP:
        {
            switch(node->data.op)
            {
                #include "../include/Operators.h"
                default:
                {
                    LOG("Unknown operator.\n");
                    return NULL;
                }
            }
            break;
        }
        case VAL:
        {
            ans =  __VAL(0);
            break;
        }
        case VAR:
        {
            ans = __VAL(VAR_EQ(node->data.var, var));
            break;
        }
        case UND: //fall through
        default:
        {
            LOG("Unknown node type.\n");
            return NULL;
        }
    }

    if(file)
    {
        PrintRandMessage(file);
        TexDerivEq(file, node, ans);
    }

    return ans;
}
#undef DEF_OP

Tree Derivative(Tree *const tree, const char *const var, FILE *file)
{
    TREE_VERIFICATION(tree, {});

    Tree deriv  = {};
    deriv.table = tree->table;

    if(file)
    {
        fprintf(file, "\\section{Дифференцирование}\n"
                      "Продифференцируем выражение:\n");
        TreeTex(tree, file);
    }

    if(!VariablesParsing(tree->table, var))
    {
        deriv.root = NodeCtor({}, VAL);
        deriv.size = 1;
    }
    else
    {
        deriv.root = SubTreeDerivative(tree->root, var, file);
        deriv.size = SubTreeSize(deriv.root);
    }

    if(file)
    {
        PrintRandMessage(file, "Значит:");
        TexDerivEq(file, tree->root, deriv.root);
    }

    return deriv;
}


Tree TaylorSeries(Tree *expr, const char *var_name, const double point, const size_t n)
{
    TREE_VERIFICATION(expr, {});

    Variable *var = VariablesParsing(expr->table, var_name);
    double val_prev = var->val;
    var->val        = point;

    Tree Taylor = {__VAL(TREE_EVAL(expr)), expr->table, 1};

    Tree temp      = {};
    Tree tempDiffn = {__CPY(expr->root), expr->table, expr->size};

    const size_t size_diff = 8;
    size_t factorial       = 1;

    for(size_t power = 1; power <= n; power++)
    {
        factorial *= power;

        TreeSimplify(&tempDiffn);
        temp = Derivative(&tempDiffn, var_name);

        Taylor.root = __ADD(Taylor.root, __MUL(__VAL(TREE_EVAL(&temp) / (double)factorial),
                                               __POW(__SUB(__VAR(var->name), __VAL(var->val)), __VAL((double)power))));
        Taylor.size += size_diff;

        TreeDtor(&tempDiffn, tempDiffn.root);
        tempDiffn = temp;
    }
    TreeDtor(&temp, temp.root);

    var->val = val_prev;

    TreeSimplify(&Taylor);
    return Taylor;
}

Tree TaylorAproximationPlot(Tree *expr, const char *var_name, const double point, const size_t n, double x_min, double x_max,
                                                                                                  double y_min, double y_max, const char *png_name)
{
    TREE_VERIFICATION(expr, {});

    TreePlot(START, x_min, x_max, y_min, y_max, png_name);
    TreePlot(PLOT, expr, NULL);

    const size_t title_max_len = 100;
    char title[title_max_len] = {};

    Variable *var = VariablesParsing(expr->table, var_name);
    double val_prev = var->val;
    var->val        = point;

    Tree Taylor = {__VAL(TREE_EVAL(expr)), expr->table, 1};
    TreePlot(PLOT, &Taylor, "o(1)");

    Tree temp      = {};
    Tree tempDiffn = {__CPY(expr->root), expr->table, expr->size};

    size_t size_diff = 8;
    size_t factorial = 1;

    for(size_t power = 1; power <= n; power++)
    {
        factorial *= power;

        TreeSimplify(&tempDiffn);
        temp = Derivative(&tempDiffn, var_name);

        Taylor.root = __ADD(Taylor.root, __MUL(__VAL(TREE_EVAL(&temp) / (double)factorial),
                                         __POW(__SUB(__VAR(var->name), __VAL(var->val)), __VAL((double)power))));
        Taylor.size += size_diff;

        sprintf(title, "o((%s - %lg)^%zu)", var_name, point, power);
        TreePlot(PLOT, &Taylor, title);

        TreeDtor(&tempDiffn, tempDiffn.root);
        tempDiffn = temp;
    }
    TreeDtor(&temp, temp.root);

    var->val = val_prev;

    TreePlot(END);

    TreeSimplify(&Taylor);
    return Taylor;
}

Tree TaylorDifferencePlot(Tree *expr, const char *var_name, const double point, const size_t n, double x_min, double x_max,
                                                                                                double y_min, double y_max, const char *png_name)
{
    TREE_VERIFICATION(expr, {});

    TreePlot(START, x_min, x_max, y_min, y_max, png_name);
    TreePlot(PLOT, expr, NULL);

    const size_t title_max_len = 100;
    char title[title_max_len] = {};

    Variable *var = VariablesParsing(expr->table, var_name);
    double val_prev = var->val;
    var->val        = point;

    Tree TreeDif = {__SUB(__CPY(expr->root), __VAL(TREE_EVAL(expr))), expr->table, expr->size + 2};
    TreePlot(PLOT, &TreeDif, "o(1)");

    Tree temp      = {};
    Tree tempDiffn = {__CPY(expr->root), expr->table, expr->size};

    size_t size_diff = 8;
    size_t factorial = 1;

    for(size_t power = 1; power <= n; power++)
    {
        factorial *= power;

        TreeSimplify(&tempDiffn);
        temp = Derivative(&tempDiffn, var_name);

        TreeDif.root->right = __ADD(TreeDif.root->right, __MUL(__VAL(TREE_EVAL(&temp) / (double)factorial),
                                                         __POW(__SUB(__VAR(var->name), __VAL(var->val)), __VAL((double)power))));
        TreeDif.size += size_diff;

        sprintf(title, "o((%s - %lg)^%zu)", var_name, point, power);
        TreePlot(PLOT, &TreeDif, title);

        TreeDtor(&tempDiffn, tempDiffn.root);
        tempDiffn = temp;
    }
    TreeDtor(&temp, temp.root);

    var->val = val_prev;

    TreePlot(END);

    TreeSimplify(&TreeDif);
    return TreeDif;
}


static bool SubTreeSearchVar(Node *const node, VariablesTable *table)
{
    if(!node) return false;

    switch(node->type)
    {
        case VAR:
        {
            for(size_t i = 0; i < N_CONSTANTS; i++)
            {
                if(node->data.var == table->vars[i].name)
                {
                    return false;
                }
            }
            return true;
        }
        case OP:
        {
            return (SubTreeSearchVar(node->left , table) ||
                    SubTreeSearchVar(node->right, table));
        }
        case VAL:
        {
            return false;
        }
        case UND: //fall through
        default:
        {
            LOG("Unknown node type.\n");
            return false;
        }
    }
}

#define DEF_OP(enum_name, e_code, dump, tex, plot, eval, diff, simplify, op_cmp) case enum_name: simplify
static void SubTreeSimplify(Tree *tree, Node *node, FILE *file, bool *changes)
{
    if(!node) return;

    switch(node->type)
    {
        case OP:
        {
            Node *copy = NULL;

            if(!HAS_VAR(node))
            {
                copy = __VAL(SubTreeCalculate(tree, node));
            }
            else
            {
                switch(node->data.op)
                {
                    #include "../include/Operators.h"
                    default:
                    {
                        LOG("Unknown operator.\n");
                        return;
                    }
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

                *node = *copy;
                tree->size += SubTreeSize(node) - 1;

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
        case VAL: return;
        case VAR: return;
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
        fprintf(file, "\\section{Упрощение}\n"
                      "Упростим выражение: \n");
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