%define api.namespace {Calc}
%define api.parser.class {Parser}
%define api.value.type {int}

%token NUMBER
%token PLUS

%%

expr:
      NUMBER          { std::cout << "Result: " << $1 << std::endl; }
    | expr PLUS NUMBER { std::cout << "Result: " << $1 + $3 << std::endl; }
    ;
%%
