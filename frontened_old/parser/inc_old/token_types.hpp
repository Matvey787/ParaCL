#pragma once

namespace ParaCL
{

enum class binary_op_t
{
    AND,
    OR,
    NOT,
    ADD,
    SUB,
    MUL,
    DIV,
    REM,
    ISAB,
    ISABE,
    ISLS,
    ISLSE,
    ISEQ,
    ISNE,
};

enum class unary_op_t
{
    MINUS,
    PLUS,
    NOT,
};

enum class combined_assign_t
{
    ADDASGN, /* += */
    SUBASGN, /* -= */
    MULASGN, /* *= */
    DIVASGN, /* /= */
    REMASGN, /* %= */
};

} /* namespace ParaCL */
