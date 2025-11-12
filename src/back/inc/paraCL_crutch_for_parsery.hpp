#ifndef PARACL_CRUTCH_FOR_PARSERY
#define PARACL_CRUTCH_FOR_PARSERY

#include <unordered_map>
#include <string>
#include <vector>
#include <string>
#include <memory>

namespace ParaCL {

enum class token_t
{
    // +, -, *, /  tokens
    ADD,
    SUB,
    MUL,
    DIV,

    // >, >=, <, <=, ==  tokens
    ISAB,
    ISABE,
    ISLS,
    ISLSE,
    ISEQ,

    // (, ), {, }  tokens
    LCIB,
    RCIB,
    LCUB,
    RCUB,

    // while, input, =, print  tokens
    WH,
    IN,
    AS,
    PRINT,

    // number, variable, semicolon  tokens 
    NUM,
    VAR,
    SC,

    // end of translation  token
    EOT
};

const std::unordered_map<std::string, token_t> tokenMap =
{
    { "+",     token_t::ADD   },
    { "-",     token_t::SUB   },
    { "*",     token_t::MUL   },
    { "/",     token_t::DIV   },

    { ">",     token_t::ISAB  },
    { ">=",    token_t::ISABE },
    { "<",     token_t::ISLS  },
    { "<=",    token_t::ISLSE },
    { "==",    token_t::ISEQ  },

    { "(",     token_t::LCIB  },
    { ")",     token_t::RCIB  },
    { "{",     token_t::LCUB  },
    { "}",     token_t::RCUB  },

    { "while", token_t::WH    },
    { "?",     token_t::IN    },
    { "=",     token_t::AS    },
    { "print", token_t::PRINT },

    { ";",     token_t::SC    }
};

const std::unordered_map<token_t, std::string> reverseTokenMap = []
{
    std::unordered_map<token_t, std::string> rev;

    for (const auto& [key, val] : tokenMap) rev[val] = key;
    
    return rev;
} ();



struct ASTNode {
    virtual ~ASTNode() = default;
};

// expressions
struct Expr : ASTNode {};

struct NumExpr : Expr {
    int value;
    NumExpr(int v) : value(v) {}
};

struct VarExpr : Expr {
    std::string name;
    VarExpr(std::string n) : name(std::move(n)) {}
};

struct InputExpr : Expr {};

struct BinExpr : Expr {
    token_t op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    BinExpr(token_t op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs)
        : op(op), left(std::move(lhs)), right(std::move(rhs)) {}
};

struct AssignExpr : Expr {
    std::string name;
    std::unique_ptr<Expr> value;
    AssignExpr(std::string n, std::unique_ptr<Expr> v)
        : name(std::move(n)), value(std::move(v)) {}
};

// statements
struct Stmt : ASTNode {};

struct AssignStmt : Stmt {
    std::string name;
    std::unique_ptr<Expr> value;
    AssignStmt(std::string n, std::unique_ptr<Expr> v)
        : name(std::move(n)), value(std::move(v)) {}
};

struct PrintStmt : Stmt {
    std::unique_ptr<Expr> expr;
    PrintStmt(std::unique_ptr<Expr> e) : expr(std::move(e)) {}
};

struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;
    BlockStmt() = default;
    BlockStmt(std::vector<std::unique_ptr<Stmt>> stmts)
        : statements(std::move(stmts)) {}
};

struct WhileStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<BlockStmt> body;
    WhileStmt(std::unique_ptr<Expr> cond, std::unique_ptr<BlockStmt> b)
        : condition(std::move(cond)), body(std::move(b)) {}
};

struct ProgramAST {
    std::vector<std::unique_ptr<Stmt>> statements;
};

void
compile(const ParaCL::ProgramAST& progAST);
    
void
dump(ProgramAST& progAST, const std::string& filename = "imgs/ast.dot");

}; // namespace ParaCL

#endif // PARACL_CRUTCH_FOR_PARSERY