#ifndef MATH_DSL_H
#define MATH_DSL_H

#define __VAL(value) NodeCtor({.num = value}, VAL)
#define __VAR(varia) NodeCtor({.var = varia}, VAR)

#define __ADD(...)  NodeCtor({.op = ADD}, OP, __VA_ARGS__)
#define __SUB(...)  NodeCtor({.op = SUB}, OP, __VA_ARGS__)
#define __MUL(...)  NodeCtor({.op = MUL}, OP, __VA_ARGS__)
#define __DIV(...)  NodeCtor({.op = DIV}, OP, __VA_ARGS__)
#define __COS(node) NodeCtor({.op = COS}, OP, NULL, node)
#define __SIN(node) NodeCtor({.op = SIN}, OP, NULL, node)


#define DIF(node) SubTreeDerivative(node, var)
#define CPY(node) SubTreeCopy(node)

#define VAL_T(type) (type == VAL)
#define VAR_T(type) (type == VAR)

#define CALC(node) SubTreeCalculate(tree, node)

#define DBL_EQ(dbl1, dbl2) (abs(dbl1 - dbl2) < M_ERR)
#define VAR_EQ(var1, var2) (strcmp(var1, var2) == 0)

#define HAS_VAR(node) SubTreeSearchVar(node, tree->table)


#define DIF_ADD return __ADD(DIF(node->left), DIF(node->right));
#define DIF_SUB return __SUB(DIF(node->left), DIF(node->right));
#define DIF_MUL return __ADD(__MUL(CPY(node->right), DIF(node->left)),\
                             __MUL(DIF(node->right), CPY(node->left)));
#define DIF_DIV return __DIV(__SUB(__MUL(CPY(node->right), DIF(node->left)),\
                                   __MUL(DIF(node->right), CPY(node->left))),\
                             __MUL(CPY(node->right), CPY(node->right)));
#define DIF_SIN return __MUL(__COS(CPY(node->right)), DIF(node->right));
#define DIF_COS return __MUL(__MUL(__VAL(-1), __SIN(CPY(node->right))),\
                               DIF(node->right));

#define EVAL_ADD return calc_left + calc_right
#define EVAL_SUB return calc_left - calc_right
#define EVAL_MUL return calc_left * calc_right
#define EVAL_DIV return calc_left / calc_right
#define EVAL_SIN return sin(calc_right)
#define EVAL_COS return cos(calc_right)

#define SMPL_ADD if(VAR_T(node->left->type) && VAR_T(node->right->type) && VAR_EQ(node->left->data.var, node->right->data.var))\
                 {\
                     copy = __MUL(__VAL(2), __VAR(node->left->data.var));\
                 }\
                 else if(VAL_T(node->left->type) && DBL_EQ(node->left->data.num, 0))\
                 {\
                     copy = CPY(node->right);\
                 }\
                 else if(VAL_T(node->right->type) && DBL_EQ(node->right->data.num, 0))\
                 {\
                     copy = CPY(node->left);\
                 }
#define SMPL_SUB if(VAR_T(node->left->type) && VAR_T(node->right->type) && VAR_EQ(node->left->data.var, node->right->data.var))\
                 {\
                     copy = __VAL(0);\
                 }\
                 else if(VAL_T(node->left->type) && DBL_EQ(node->left->data.num, 0))\
                 {\
                     copy = CPY(node->right);\
                 }\
                 else if(VAL_T(node->right->type) && DBL_EQ(node->right->data.num, 0))\
                 {\
                     copy = CPY(node->left);\
                 }
#define SMPL_MUL if((VAL_T(node->left->type)  && DBL_EQ(node->left->data.num , 0)) ||\
                    (VAL_T(node->right->type) && DBL_EQ(node->right->data.num, 0)))\
                 {\
                     copy = __VAL(0);\
                 }\
                 else if(VAL_T(node->left->type) && DBL_EQ(node->left->data.num, 1))\
                 {\
                     copy = CPY(node->right);\
                 }\
                 else if(VAL_T(node->right->type) && DBL_EQ(node->right->data.num, 1))\
                 {\
                     copy = CPY(node->left);\
                 }
#define SMPL_DIV if(VAR_T(node->left->type) && VAR_T(node->right->type) && VAR_EQ(node->left->data.var, node->right->data.var))\
                 {\
                     copy = __VAL(1);\
                 }\
                 else if(VAL_T(node->left->type) && DBL_EQ(node->left->data.num, 0))\
                 {\
                     copy = __VAL(0);\
                 }\
                 else if(VAL_T(node->right->type) && DBL_EQ(node->right->data.num, 1))\
                 {\
                     copy = CPY(node->left);\
                 }
#define SMPL_SIN ;
#define SMPL_COS ;

#endif //MATH_DSL_H