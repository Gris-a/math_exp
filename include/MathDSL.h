#ifndef MATH_DSL_H
#define MATH_DSL_H

//General
#define DIF(node) SubTreeDerivative(node, var, file)
#define CPY(node) SubTreeCopy(node)

#define VAL_T(type) (type == VAL)
#define VAR_T(type) (type == VAR)
#define OP_T(type)  (type == OP)

#define CALC(node) SubTreeCalculate(tree, node)

#define DBL_EQ(dbl1, dbl2) (abs(dbl1 - dbl2) < M_ERR)
#define VAR_EQ(var1, var2) (strcmp(var1, var2) == 0)

#define HAS_VAR(node) SubTreeSearchVar(node, tree->table)

#define LEFT node->left
#define RIGHT node->right



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

#endif //MATH_DSL_H