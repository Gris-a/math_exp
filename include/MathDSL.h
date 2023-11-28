#ifndef MATH_DSL_H
#define MATH_DSL_H

//General
#define DIF(node) SubTreeDerivative(node, var, tex)
#define CPY(node) SubTreeCopy(node)

#define VAL_T(type) (type == VAL)
#define VAR_T(type) (type == VAR)
#define OP_T(type)  (type == OP)

#define CALC(node) SubTreeCalculate(tree, node)

#define DBL_EQ(dbl1, dbl2) (abs(dbl1 - dbl2) < M_ERR)
#define VAR_EQ(var1, var2) (strcmp(var1, var2) == 0)

#define HAS_VAR(node) SubTreeSearchVar(node, tree->table)




//Node generator
#define __VAL(value)  NodeCtor({.num = (value)}, VAL)
#define __VAR(varia)  NodeCtor({.var = (varia)}, VAR)

#define __ADD(...)    NodeCtor({.op = ADD }, OP, __VA_ARGS__)
#define __SUB(...)    NodeCtor({.op = SUB }, OP, __VA_ARGS__)
#define __MUL(...)    NodeCtor({.op = MUL }, OP, __VA_ARGS__)
#define __DIV(...)    NodeCtor({.op = DIV }, OP, __VA_ARGS__)
#define __POW(...)    NodeCtor({.op = POW }, OP, __VA_ARGS__)
#define __LN(node)    NodeCtor({.op = LN  }, OP, NULL, node )
#define __SIN(node)   NodeCtor({.op = SIN }, OP, NULL, node )
#define __COS(node)   NodeCtor({.op = COS }, OP, NULL, node )
#define __TG(node)    NodeCtor({.op = TG  }, OP, NULL, node )
#define __ASIN(node)  NodeCtor({.op = ASIN}, OP, NULL, node )
#define __ACOS(node)  NodeCtor({.op = ACOS}, OP, NULL, node )
#define __ATG(node)   NodeCtor({.op = ATG }, OP, NULL, node )



//Operators comparator. true if need parathenses
#define OP_CMP_ADD  return false
#define OP_CMP_SUB  if(!is_l_child && ((node_op == ADD) || (node_op == SUB))) return true;\
                    else return false
#define OP_CMP_MUL  if((node_op == ADD) || (node_op == SUB)) return true;\
                    else return false
#define OP_CMP_DIV  if((node_op == ADD) || (node_op == SUB) || (!is_l_child && ((node_op == DIV) || (node_op == MUL)))) return true;\
                    else return false
#define OP_CMP_POW  if((node_op == ADD) || (node_op == SUB) || (node_op == DIV) || (node_op == MUL) || (is_l_child && node_op == POW)) return true;\
                    else return false
#define OP_CMP_SIN  return true
#define OP_CMP_COS  return true
#define OP_CMP_TG   return true
#define OP_CMP_ASIN return true
#define OP_CMP_ACOS return true
#define OP_CMP_ATG  return true
#define OP_CMP_LN   return true




//Deffirenciation formulas
#define DIF_ADD  ans = __ADD(DIF(node->left), DIF(node->right))
#define DIF_SUB  ans = __SUB(DIF(node->left), DIF(node->right))
#define DIF_MUL  ans = __ADD(__MUL(CPY(node->right), DIF(node->left)),\
                             __MUL(DIF(node->right), CPY(node->left)))
#define DIF_DIV  ans = __DIV(__SUB(__MUL(CPY(node->right), DIF(node->left)),\
                                   __MUL(DIF(node->right), CPY(node->left))),\
                             __MUL(CPY(node->right), CPY(node->right)))
#define DIF_POW  if(VAL_T(node->right->type))\
                 {\
                     ans = __MUL(__POW(CPY(node->left), __VAL(node->right->data.num - 1)),\
                                 __MUL(__VAL(node->right->data.num), DIF(node->left)));\
                 }\
                 else\
                 {\
                     ans = __MUL(CPY(node), __ADD(__DIV(__MUL(CPY(node->right), DIF(node->left)), CPY(node->left)),\
                                                  __MUL(DIF(node->right), __LN(CPY(node->left)))));\
                 }
#define DIF_LN   ans = __MUL(__POW(CPY(node->right), __VAL(-1)),\
                               DIF(node->right))
#define DIF_SIN  ans = __MUL(__COS(CPY(node->right)), DIF(node->right))
#define DIF_COS  ans = __MUL(__MUL(__VAL(-1), __SIN(CPY(node->right))),\
                                DIF(node->right))
#define DIF_TG   ans = __MUL(__POW(__COS(CPY(node->right)), __VAL(-2)),\
                               DIF(node->right))
#define DIF_ASIN ans = __MUL(__POW(__SUB(__VAL(1), __POW(CPY(node->right), __VAL(2))), __VAL(-0.5)),\
                               DIF(node->right))
#define DIF_ACOS ans = __MUL(__VAL(-1), __MUL(__POW(__SUB(__VAL(1), __POW(CPY(node->right), __VAL(2))), __VAL(-0.5)),\
                                                DIF(node->right)))
#define DIF_ATG  ans = __MUL(__POW(__ADD(__VAL(1), __POW(CPY(node->right), __VAL(2))), __VAL(-1)),\
                               DIF(node->right))




//Calculations
#define EVAL_ADD  return calc_left + calc_right
#define EVAL_SUB  return calc_left - calc_right
#define EVAL_MUL  return calc_left * calc_right
#define EVAL_DIV  return calc_left / calc_right
#define EVAL_POW  return pow(calc_left, calc_right)
#define EVAL_LN   return log(calc_right)
#define EVAL_SIN  return sin(calc_right)
#define EVAL_COS  return cos(calc_right)
#define EVAL_TG   return tan(calc_right)
#define EVAL_ASIN return asin(calc_right)
#define EVAL_ACOS return acos(calc_right)
#define EVAL_ATG  return atan(calc_right)



//Simplifying of tree
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
#define SMPL_SUB if(OP_T(node->right->type))\
                 {\
                    if(node->right->data.op == SUB)\
                    {\
                        copy = __ADD(__SUB(CPY(node->left), CPY(node->right->left)), CPY(node->right->right));\
                    }\
                    else if(node->right->data.op == ADD)\
                    {\
                        copy = __SUB(__SUB(CPY(node->left), CPY(node->right->left)), CPY(node->right->right));\
                    }\
                 }\
                 else if(VAR_T(node->left->type) && VAR_T(node->right->type) && VAR_EQ(node->left->data.var, node->right->data.var))\
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
#define SMPL_DIV if(OP_T(node->right->type) && (node->right->data.op == DIV))\
                 {\
                     copy = __MUL(__DIV(CPY(node->left), CPY(node->right->left)), CPY(node->right->right));\
                 }\
                 else if(VAR_T(node->left->type) && VAR_T(node->right->type) && VAR_EQ(node->left->data.var, node->right->data.var))\
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
#define SMPL_POW if(OP_T(node->left->type) && node->left->data.op == POW)\
                 {\
                     copy = __POW(CPY(node->left->left), __MUL(CPY(node->left->right), CPY(node->right)));\
                 }\
                 else if((VAL_T(node->left->type)  && DBL_EQ(node->left->data.num , 1)) ||\
                    (VAL_T(node->right->type) && DBL_EQ(node->right->data.num, 0)))\
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
#define SMPL_LN
#define SMPL_SIN if(OP_T(node->right->type) && node->right->data.op == ASIN)\
                 {\
                    copy = CPY(node->right->right);\
                 }
#define SMPL_COS if(OP_T(node->right->type) && node->right->data.op == ACOS)\
                 {\
                    copy = CPY(node->right->right);\
                 }
#define SMPL_TG  if(OP_T(node->right->type) && node->right->data.op == ATG )\
                 {\
                    copy = CPY(node->right->right);\
                 }
#define SMPL_ASIN
#define SMPL_ACOS
#define SMPL_ATG

#endif //MATH_DSL_H