#ifndef MATH_DSL_H
#define MATH_DSL_H

//General
#define __DIF(node) SubTreeDerivative(node, var, file)
#define __CPY(node) SubTreeCopy(node)

#define VAL_T(node) (node->type == VAL)
#define VAR_T(node) (node->type == VAR)
#define OP_T(node)  (node->type == OP)

#define TREE_EVAL(tree) TreeCalculate(tree)

#define DBL_EQ(dbl1, dbl2) (abs(dbl1 - dbl2) < M_ERR)
#define IS_NODE_ZERO(node) (VAL_T(node) && DBL_EQ(node->data.num , 0))
#define IS_NODE_ONE(node)  (VAL_T(node) && DBL_EQ(node->data.num , 1))

#define VAR_EQ(var1, var2) (strcmp(var1, var2) == 0)
#define IS_NODES_SAME_VAR(node1, node2) (VAR_T(LEFT) && VAR_T(RIGHT) && VAR_EQ(LEFT->data.var, RIGHT->data.var))

#define OP_EQ(op1, op2) (op1 == op2)
#define IS_NODE_OP(node, operator) (OP_T(node) && OP_EQ(node->data.op, operator))

#define HAS_VAR(node) SubTreeSearchVar(node, tree->table)

#define LEFT  node->left
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