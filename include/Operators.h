/*


DEF_OP(enum_name, (((enum_code << 1) + is_prefix_tex) << 1) + is_function, literal, tex_literal, plot_literal,
{
    evaluate
},
{
    __DIFferenciate
},
{
    simplify
},
{
    operators_comparasion
})
*/

DEF_OP(ADD, (((0 << 1) + 0) << 1) + 0, "+", " + ", "+",
{
    return lvalue + rvalue;
},
{
    ans = __ADD(__DIF(LEFT), __DIF(RIGHT));
    break;
},
{
    if(IS_NODES_SAME_VAR(LEFT, RIGHT))
    {
        copy = __MUL(__VAL(2), __VAR(LEFT->data.var));
    }
    else if(IS_NODE_ZERO(LEFT))
    {
        copy = __CPY(RIGHT);
    }
    else if(IS_NODE_ZERO(RIGHT))
    {
        copy = __CPY(LEFT);
    }
    break;
},
{
    return false;
})


DEF_OP(SUB, (((1 << 1) + 0) << 1) + 0, "-", " - ", "-",
{
    return lvalue - rvalue;
},
{
    ans = __SUB(__DIF(LEFT), __DIF(RIGHT));
    break;
},
{
    if(IS_NODE_OP(RIGHT, SUB))
    {
        copy = __ADD(__SUB(__CPY(LEFT), __CPY(RIGHT->left)), __CPY(RIGHT->right));
    }
    else if(IS_NODE_OP(RIGHT, ADD))
    {
        copy = __SUB(__SUB(__CPY(LEFT), __CPY(RIGHT->left)), __CPY(RIGHT->right));
    }
    else if(IS_NODES_SAME_VAR(LEFT, RIGHT))
    {
        copy = __VAL(0);
    }
    else if(IS_NODE_ZERO(LEFT))
    {
        copy = __CPY(RIGHT);
    }
    else if(IS_NODE_ZERO(RIGHT))
    {
        copy = __CPY(LEFT);
    }
    break;
},
{
    if(!is_l_child && ((node_op == ADD) || (node_op == SUB))) return true;
    else return false;
})


DEF_OP(MUL, (((2 << 1) + 0) << 1) + 0, "*", "\\cdot ", "*",
{
    return lvalue * rvalue;
},
{
    ans = __ADD(__MUL(__CPY(RIGHT), __DIF(LEFT)),
                __MUL(__DIF(RIGHT), __CPY(LEFT)));
    break;
},
{
    if(IS_NODE_ZERO(LEFT) ||
       IS_NODE_ZERO(RIGHT))
    {
        copy = __VAL(0);
    }
    else if(IS_NODE_ONE(LEFT))
    {
        copy = __CPY(RIGHT);
    }
    else if(IS_NODE_ONE(RIGHT))
    {
        copy = __CPY(LEFT);
    }
    break;
},
{
    if((node_op == ADD) || (node_op == SUB)) return true;
    else return false;
})


DEF_OP(DIV, (((3 << 1) + 1) << 1) + 0, "/", "\\frac", "/",
{
    if(DBL_EQ(rvalue, 0))
    {
        LOG("Division by zero.\n");
        return NAN;
    }
    return lvalue / rvalue;
},
{
    ans = __DIV(__SUB(__MUL(__CPY(RIGHT), __DIF(LEFT)),
                      __MUL(__DIF(RIGHT), __CPY(LEFT))),
                __POW(__CPY(RIGHT), __VAL(2)));
    break;
},
{
    if(OP_T(RIGHT) && (RIGHT->data.op == DIV))
    {
        copy = __MUL(__DIV(__CPY(LEFT), __CPY(RIGHT->left)), __CPY(RIGHT->right));
    }
    else if(IS_NODES_SAME_VAR(LEFT, RIGHT))
    {
        copy = __VAL(1);
    }
    else if(IS_NODE_ZERO(LEFT))
    {
        copy = __VAL(0);
    }
    else if(IS_NODE_ONE(RIGHT))
    {
        copy = __CPY(LEFT);
    }
    break;
},
{
    if((node_op == ADD) || (node_op == SUB) || (!is_l_child && ((node_op == DIV) || (node_op == MUL)))) return true;
    else return false;
})


DEF_OP(POW, (((4 << 1) + 0) << 1) + 0, "^", " ^ ", "**",
{
    if(DBL_EQ(rvalue, round(rvalue)))
    {
        if(rvalue > 0)
        {
            return FastPow(lvalue, (long long)round(rvalue));
        }
        else
        {
            return 1 / FastPow(lvalue, -(long long)round(rvalue));
        }
    }
    else
    {
        return pow(lvalue, rvalue);
    }
},
{
    if(VAL_T(RIGHT))
    {
        ans = __MUL(__POW(__CPY(LEFT), __VAL(RIGHT->data.num - 1)),
                    __MUL(__VAL(RIGHT->data.num), __DIF(LEFT)));
    }
    else
    {
        ans = __MUL(__CPY(node), __ADD(__DIV(__MUL(__CPY(RIGHT), __DIF(LEFT)), __CPY(LEFT)),
                                       __MUL(__DIF(RIGHT), __LN(__CPY(LEFT)))));
    }
    break;
},
{
    if(IS_NODE_OP(LEFT, POW))
    {
        copy = __POW(__CPY(LEFT->left), __MUL(__CPY(LEFT->right), __CPY(RIGHT)));
    }
    else if(IS_NODE_ONE(LEFT) ||
            IS_NODE_ZERO(RIGHT))
    {
        copy = __VAL(1);
    }
    else if(IS_NODE_ZERO(LEFT))
    {
        copy = __VAL(0);
    }
    else if(IS_NODE_ONE(RIGHT))
    {
        copy = __CPY(LEFT);
    }
    break;
},
{
    if((node_op == ADD) || (node_op == SUB) || (node_op == DIV) || (node_op == MUL) || (is_l_child && node_op == POW)) return true;
    else return false;
})


DEF_OP(LN, (((5 << 1) + 1) << 1) + 1, "ln", "\\ln", "log",
{
    return log(rvalue);
},
{
    ans = __MUL(__POW(__CPY(RIGHT), __VAL(-1)),
                __DIF(RIGHT));
    break;
},
{
    break;
},
{
    return true;
})


DEF_OP(SIN, (((6 << 1) + 1) << 1) + 1, "sin", "\\sin", "sin",
{
    return sin(rvalue);
},
{
    ans = __MUL(__COS(__CPY(RIGHT)), __DIF(RIGHT));
    break;
},
{
    if(IS_NODE_OP(RIGHT, ASIN))
    {
        copy = __CPY(RIGHT->right);
    }
    break;
},
{
    return true;
})


DEF_OP(COS, (((7 << 1) + 1) << 1) + 1, "cos", "\\cos", "cos",
{
    return cos(rvalue);
},
{
    ans = __MUL(__MUL(__VAL(-1), __SIN(__CPY(RIGHT))),
                __DIF(RIGHT));
    break;
},
{
    if(IS_NODE_OP(RIGHT, ACOS))
    {
        copy = __CPY(RIGHT->right);
    }
    break;
},
{
    return true;
})


DEF_OP(TG, (((8 << 1) + 1) << 1) + 1, "tg", "\\tan", "tan",
{
    return tan(rvalue);
},
{
    ans = __MUL(__POW(__COS(__CPY(RIGHT)), __VAL(-2)),
                __DIF(RIGHT));
    break;
},
{
    if(IS_NODE_OP(RIGHT, ATG))
    {
        copy = __CPY(RIGHT->right);
    }
    break;
},
{
    return true;
})


DEF_OP(ASIN, (((9 << 1) + 1) << 1) + 1, "asin", "\\arcsin", "asin",
{
    return asin(rvalue);
},
{
    ans = __MUL(__POW(__SUB(__VAL(1), __POW(__CPY(RIGHT), __VAL(2))), __VAL(-0.5)),
                __DIF(RIGHT));
    break;
},
{
    break;
},
{
    return true;
})


DEF_OP(ACOS, (((10 << 1) + 1) << 1) + 1, "acos", "\\arccos", "acos",
{
    return acos(rvalue);
},
{
    ans = __MUL(__VAL(-1), __MUL(__POW(__SUB(__VAL(1), __POW(__CPY(RIGHT), __VAL(2))), __VAL(-0.5)),
                __DIF(RIGHT)));
    break;
},
{
    break;
},
{
    return true;
})


DEF_OP(ATG, (((11 << 1) + 1) << 1) + 1, "atg", "\\arctan", "atan",
{
    return atan(rvalue);
},
{
    ans = __MUL(__POW(__ADD(__VAL(1), __POW(__CPY(RIGHT), __VAL(2))), __VAL(-1)),
                __DIF(RIGHT));
    break;
},
{
    break;
},
{
    return true;
})