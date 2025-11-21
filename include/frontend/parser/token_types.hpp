#pragma once

namespace ParaCL
{
enum class token_t
{
    // and, or, not tokens
    AND,
    OR,
    NOT,

    // +, -, *,  /, %  tokens
    ADD,
    SUB,
    MUL,
    DIV,
    REM,

    // combined assignment operators
    ADDASGN, /* += */
    SUBASGN, /* -= */
    MULASGN, /* *= */
    DIVASGN, /* /= */
    REMASGN, /* %= */
    INC,     /* ++ */
    DEC,     /* -- */

    // >, >=, <, <=, ==, !=  tokens
    ISAB,
    ISABE,
    ISLS,
    ISLSE,
    ISEQ,
    ISNE,

    // (, ), {, }  tokens
    LCIB,
    RCIB,
    LCUB,
    RCUB,

    // while, input, =, print, if, else, else if tokens
    WH,
    IN,
    AS,
    PRINT,

    IF,
    ELSE,
    ELIF,

    // number, variable, semicolon, comma tokens
    NUM,
    VAR,
    STRING,
    SC,
    COMMA,

    // end of translation token
    EOT
};

} /* namespace ParaCL */
