#include "MathDSL.h"


//DEF_OP(enum_name, (((enum_code << 1) + is_prefix_tex) << 1) + is_function, literal, tex_literal, evaluate, differenciate, simplify, operators_comparasion)
DEF_OP(ADD , (((0  << 1) + 0) << 1) + 0, "+"   , " + "       , EVAL_ADD , DIF_ADD , SMPL_ADD , OP_CMP_ADD )
DEF_OP(SUB , (((1  << 1) + 0) << 1) + 0, "-"   , " - "       , EVAL_SUB , DIF_SUB , SMPL_SUB , OP_CMP_SUB )
DEF_OP(MUL , (((2  << 1) + 0) << 1) + 0, "*"   , " \\cdot "  , EVAL_MUL , DIF_MUL , SMPL_MUL , OP_CMP_MUL )
DEF_OP(DIV , (((3  << 1) + 1) << 1) + 0, "/"   , " \\frac "  , EVAL_DIV , DIF_DIV , SMPL_DIV , OP_CMP_DIV )
DEF_OP(POW , (((4  << 1) + 0) << 1) + 0, "^"   , " ^ "       , EVAL_POW , DIF_POW , SMPL_POW , OP_CMP_POW )
DEF_OP(LN  , (((5  << 1) + 1) << 1) + 1, "ln"  , " \\ln "    , EVAL_LN  , DIF_LN  , SMPL_LN  , OP_CMP_LN  )
DEF_OP(SIN , (((6  << 1) + 1) << 1) + 1, "sin" , " \\sin "   , EVAL_SIN , DIF_SIN , SMPL_SIN , OP_CMP_SIN )
DEF_OP(COS , (((7  << 1) + 1) << 1) + 1, "cos" , " \\cos "   , EVAL_COS , DIF_COS , SMPL_COS , OP_CMP_COS )
DEF_OP(TG  , (((8  << 1) + 1) << 1) + 1, "tg"  , " \\tan "   , EVAL_TG  , DIF_TG  , SMPL_TG  , OP_CMP_TG  )
DEF_OP(ASIN, (((9  << 1) + 1) << 1) + 1, "asin", " \\arcsin ", EVAL_ASIN, DIF_ASIN, SMPL_ASIN, OP_CMP_ASIN)
DEF_OP(ACOS, (((10 << 1) + 1) << 1) + 1, "acos", " \\arccos ", EVAL_ACOS, DIF_ACOS, SMPL_ACOS, OP_CMP_ACOS)
DEF_OP(ATG , (((11 << 1) + 1) << 1) + 1, "atg" , " \\arctan ", EVAL_ATG , DIF_ATG , SMPL_ATG , OP_CMP_ATG )