#include "MathDSL.h"

DEF_OP(ADD, "+"  , EVAL_ADD, DIF_ADD, SMPL_ADD)

DEF_OP(SUB, "-"  , EVAL_SUB, DIF_SUB, SMPL_SUB)

DEF_OP(MUL, "*"  , EVAL_MUL, DIF_MUL, SMPL_MUL)

DEF_OP(DIV, "/"  , EVAL_DIV, DIF_DIV, SMPL_DIV)

DEF_OP(POW, "^"  , EVAL_POW, DIF_POW, SMPL_POW)

DEF_OP(SIN, "sin", EVAL_SIN, DIF_SIN, SMPL_SIN)

DEF_OP(COS, "cos", EVAL_COS, DIF_COS, SMPL_COS)

DEF_OP(LN , "ln" , EVAL_LN , DIF_LN , SMPL_LN )