%{
#include <string>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include "parse_error.hpp"
#include "lexer.hpp"

std::string current_file;

#define YY_DECL int yylex(yy::parser::semantic_type* yylval, yy::parser::location_type* yylloc)


#define YY_USER_ACTION                          \ 
    do                                          \ 
    {                                           \
    yylloc->begin.line   = yylloc->end.line;    \
    yylloc->begin.column = yylloc->end.column;  \
    for (char c : std::string(yytext)) {        \
        if (c == '\n') {                        \
            yylloc->end.line++;                 \
            yylloc->end.column = 1;             \
        } else if (c == '\t') {                 \
            yylloc->end.column += 4;            \
        } else {                                \
            yylloc->end.column++;               \
        }                                       \
    }                                           \
} while (0);

%}

%option noyywrap
%option nounput
%option noinput
%option yylineno

DIGIT     [0-9]
LETTER    [a-zA-Z_]
STRING    \"[^"]*\"

%%
[ \t\r]+          { }                     /* whitespace */
\n                { }                     /* newline */

"//".*            { }                     /* one-line comment */
"/*"([^*]|"*"+[^*/])*"*"+"/" { }          /* multi-line comment */
"#!".*            { }                     /* shebang */

"and"             { return yy::parser::token::AND; }
"&&"              { return yy::parser::token::AND; }
"or"              { return yy::parser::token::OR; }
"||"              { return yy::parser::token::OR; }
"!"               { return yy::parser::token::NOT; }
"not"             { return yy::parser::token::NOT; }

"+"               { return yy::parser::token::ADD; }
"-"               { return yy::parser::token::SUB; }
"*"               { return yy::parser::token::MUL; }
"/"               { return yy::parser::token::DIV; }
"%"               { return yy::parser::token::REM; }

">"               { return yy::parser::token::ISAB; }
">="              { return yy::parser::token::ISABE; }
"<"               { return yy::parser::token::ISLS; }
"<="              { return yy::parser::token::ISLSE; }
"=="              { return yy::parser::token::ISEQ; }
"!="              { return yy::parser::token::ISNE; }

"+="              { return yy::parser::token::ADDASGN; }
"-="              { return yy::parser::token::SUBASGN; }
"*="              { return yy::parser::token::MULASGN; }
"/="              { return yy::parser::token::DIVASGN; }

"("               { return yy::parser::token::LCIB; }
")"               { return yy::parser::token::RCIB; }
"{"               { return yy::parser::token::LCUB; }
"}"               { return yy::parser::token::RCUB; }

"while"           { return yy::parser::token::WH; }
"?"               { return yy::parser::token::IN; }
"="               { return yy::parser::token::AS; }
"print"           { return yy::parser::token::PRINT; }
"if"              { return yy::parser::token::IF; }
"else if"         { return yy::parser::token::ELIF; }
"else"            { return yy::parser::token::ELSE; }
";"               { return yy::parser::token::SC; }
","               { return yy::parser::token::COMMA; }

{STRING} {
    yytext[yyleng - 1] = '\0';
    yylval->build<std::string>(yytext + 1);
    return yy::parser::token::STRING;
}

{DIGIT}+ {
    yylval->build<int>(atoi(yytext));
    return yy::parser::token::NUM;
}

{LETTER}({LETTER}|{DIGIT})* {
    yylval->build<std::string>(yytext);
    return yy::parser::token::VAR;
}

<<EOF>> {
    return yy::parser::token::YYEOF;
}

. {
    std::cerr << "paracl: lexer: unexpected character '"
              << yytext << "' at "
              << current_file << ":"
              << yylloc->begin.line << ":"
              << yylloc->begin.column << std::endl;

    ErrorHandler::throwError(*yylloc,
        "unexpected character '" + std::string(yytext) + "'");
    return yy::parser::token::YYerror;
}
%%



void set_current_paracl_file(std::string_view file)
{
    current_file = std::string(file);
}
