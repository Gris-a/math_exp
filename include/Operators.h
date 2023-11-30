/*


DEF_OP(enum_name, (((enum_code << 1) + is_prefix_tex) << 1) + is_function, literal, tex_literal, plot_literal,
{
    evaluate
},
{
    differenciate
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
    return calc_left + calc_right;
},
{
    ans = __ADD(DIF(LEFT), DIF(RIGHT));
    break;
},
{
    if(VAR_T(LEFT->type) && VAR_T(RIGHT->type) && VAR_EQ(LEFT->data.var, RIGHT->data.var))
    {
        copy = __MUL(__VAL(2), __VAR(LEFT->data.var));
    }
    else if(VAL_T(LEFT->type) && DBL_EQ(LEFT->data.num, 0))
    {
        copy = CPY(RIGHT);
    }
    else if(VAL_T(RIGHT->type) && DBL_EQ(RIGHT->data.num, 0))
    {
        copy = CPY(LEFT);
    }
    break;
},
{
    return false;
})


DEF_OP(SUB, (((1 << 1) + 0) << 1) + 0, "-", " - ", "-",
{
    return calc_left - calc_right;
},
{
    ans = __SUB(DIF(LEFT), DIF(RIGHT));
    break;
},
{
    if(OP_T(RIGHT->type))
    {
    if(RIGHT->data.op == SUB)
    {
        copy = __ADD(__SUB(CPY(LEFT), CPY(RIGHT->left)), CPY(RIGHT->right));
    }
    else if(RIGHT->data.op == ADD)
    {
        copy = __SUB(__SUB(CPY(LEFT), CPY(RIGHT->left)), CPY(RIGHT->right));
    }
    }
    else if(VAR_T(LEFT->type) && VAR_T(RIGHT->type) && VAR_EQ(LEFT->data.var, RIGHT->data.var))
    {
        copy = __VAL(0);
    }
    else if(VAL_T(LEFT->type) && DBL_EQ(LEFT->data.num, 0))
    {
        copy = CPY(RIGHT);
    }
    else if(VAL_T(RIGHT->type) && DBL_EQ(RIGHT->data.num, 0))
    {
        copy = CPY(LEFT);
    }
    break;
},
{
    if(!is_l_child && ((node_op == ADD) || (node_op == SUB))) return true;
    else return false;
})


DEF_OP(MUL, (((2 << 1) + 0) << 1) + 0, "*", "\\cdot ", "*",
{
    return calc_left * calc_right;
},
{
    ans = __ADD(__MUL(CPY(RIGHT), DIF(LEFT)),
                __MUL(DIF(RIGHT), CPY(LEFT)));
    break;
},
{
    if((VAL_T(LEFT->type)  && DBL_EQ(LEFT->data.num , 0)) ||
       (VAL_T(RIGHT->type) && DBL_EQ(RIGHT->data.num, 0)))
    {
        copy = __VAL(0);
    }
    else if(VAL_T(LEFT->type) && DBL_EQ(LEFT->data.num, 1))
    {
        copy = CPY(RIGHT);
    }
    else if(VAL_T(RIGHT->type) && DBL_EQ(RIGHT->data.num, 1))
    {
        copy = CPY(LEFT);
    }
    break;
},
{
    if((node_op == ADD) || (node_op == SUB)) return true;
    else return false;
})


DEF_OP(DIV, (((3 << 1) + 1) << 1) + 0, "/", "\\frac", "/",
{
    if(DBL_EQ(calc_right, 0))
    {
        LOG("Division by zero.\n");
        return NAN;
    }
    return calc_left / calc_right;
},
{
    ans = __DIV(__SUB(__MUL(CPY(RIGHT), DIF(LEFT)),
                      __MUL(DIF(RIGHT), CPY(LEFT))),
                __POW(CPY(RIGHT), __VAL(2)));
    break;
},
{
    if(OP_T(RIGHT->type) && (RIGHT->data.op == DIV))
    {
        copy = __MUL(__DIV(CPY(LEFT), CPY(RIGHT->left)), CPY(RIGHT->right));
    }
    else if(VAR_T(LEFT->type) && VAR_T(RIGHT->type) && VAR_EQ(LEFT->data.var, RIGHT->data.var))
    {
        copy = __VAL(1);
    }
    else if(VAL_T(LEFT->type) && DBL_EQ(LEFT->data.num, 0))
    {
        copy = __VAL(0);
    }
    else if(VAL_T(RIGHT->type) && DBL_EQ(RIGHT->data.num, 1))
    {
        copy = CPY(LEFT);
    }
    break;
},
{
    if((node_op == ADD) || (node_op == SUB) || (!is_l_child && ((node_op == DIV) || (node_op == MUL)))) return true;
    else return false;
})


DEF_OP(POW, (((4 << 1) + 0) << 1) + 0, "^", " ^ ", "**",
{
    return pow(calc_left, calc_right);
},
{
    if(VAL_T(RIGHT->type))
    {
        ans = __MUL(__POW(CPY(LEFT), __VAL(RIGHT->data.num - 1)),
                    __MUL(__VAL(RIGHT->data.num), DIF(LEFT)));
    }
    else
    {
        ans = __MUL(CPY(node), __ADD(__DIV(__MUL(CPY(RIGHT), DIF(LEFT)), CPY(LEFT)),
                                     __MUL(DIF(RIGHT), __LN(CPY(LEFT)))));
    }
    break;
},
{
    if(OP_T(LEFT->type) && LEFT->data.op == POW)
    {
        copy = __POW(CPY(LEFT->left), __MUL(CPY(LEFT->right), CPY(RIGHT)));
    }
    else if((VAL_T(LEFT->type)  && DBL_EQ(LEFT->data.num , 1)) ||
            (VAL_T(RIGHT->type) && DBL_EQ(RIGHT->data.num, 0)))
    {
        copy = __VAL(1);
    }
    else if(VAL_T(LEFT->type) && DBL_EQ(LEFT->data.num, 0))
    {
        copy = __VAL(0);
    }
    else if(VAL_T(RIGHT->type) && DBL_EQ(RIGHT->data.num, 1))
    {
        copy = CPY(LEFT);
    }
    break;
},
{
    if((node_op == ADD) || (node_op == SUB) || (node_op == DIV) || (node_op == MUL) || (is_l_child && node_op == POW)) return true;
    else return false;
})


DEF_OP(LN, (((5 << 1) + 1) << 1) + 1, "ln", "\\ln", "log",
{
    return log(calc_right);
},
{
    ans = __MUL(__POW(CPY(RIGHT), __VAL(-1)),
                DIF(RIGHT));
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
    return sin(calc_right);
},
{
    ans = __MUL(__COS(CPY(RIGHT)), DIF(RIGHT));
    break;
},
{
    if(OP_T(RIGHT->type) && RIGHT->data.op == ASIN)
    {
        copy = CPY(RIGHT->right);
    }
    break;
},
{
    return true;
})


DEF_OP(COS, (((7 << 1) + 1) << 1) + 1, "cos", "\\cos", "cos",
{
    return cos(calc_right);
},
{
    ans = __MUL(__MUL(__VAL(-1), __SIN(CPY(RIGHT))),
                DIF(RIGHT));
    break;
},
{
    if(OP_T(RIGHT->type) && RIGHT->data.op == ACOS)
    {
        copy = CPY(RIGHT->right);
    }
    break;
},
{
    return true;
})


DEF_OP(TG, (((8 << 1) + 1) << 1) + 1, "tg", "\\tan", "tan",
{
    return tan(calc_right);
},
{
    ans = __MUL(__POW(__COS(CPY(RIGHT)), __VAL(-2)),
                DIF(RIGHT));
    break;
},
{
    if(OP_T(RIGHT->type) && RIGHT->data.op == ATG)
    {
        copy = CPY(RIGHT->right);
    }
    break;
},
{
    return true;
})


DEF_OP(ASIN, (((9 << 1) + 1) << 1) + 1, "asin", "\\arcsin", "asin",
{
    return asin(calc_right);
},
{
    ans = __MUL(__POW(__SUB(__VAL(1), __POW(CPY(RIGHT), __VAL(2))), __VAL(-0.5)),
                DIF(RIGHT));
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
    return acos(calc_right);
},
{
    ans = __MUL(__VAL(-1), __MUL(__POW(__SUB(__VAL(1), __POW(CPY(RIGHT), __VAL(2))), __VAL(-0.5)),
                DIF(RIGHT)));
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
    return atan(calc_right);
},
{
    ans = __MUL(__POW(__ADD(__VAL(1), __POW(CPY(RIGHT), __VAL(2))), __VAL(-1)),
                DIF(RIGHT));
    break;
},
{
    break;
},
{
    return true;
})