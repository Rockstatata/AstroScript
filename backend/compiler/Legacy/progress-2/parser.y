%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex();
void yyerror(const char *s);
extern FILE *yyin;
extern int yylineno;
%}


%union {
    int intval;
    double floatval;
    char* str;
}

%token MISSION LAUNCH SUCCESS ABORT
%token MODULE DEPLOY EXTENDS PUBLIC PRIVATE THIS
%token TELEMETRY LIMIT ALIAS FLEET MODE
%token COMMAND BACK
%token BROADCAST RECEIVE ALARM
%token VERIFY ELSE_VERIFY OTHERWISE
%token ORBIT SCENARIO TRAJECTORY FALLBACK
%token STAGE_SEP COAST
%token WAIT TICK
%token ROOT FLR CEIL ABS LOGARITHM SINE COSINE TAN ASINE ACOSINE ATAN PRIME
%token WHILE TIMES

%token COUNT REAL PRECISE FLAG SYMBOL VOIDSPACE

%token ADD MINUS MUL DIVIDE MOD POWER
%token AND OR NOT XOR

%token LT GT LE GE EQ NEQ
%token ASSIGN
%token RETTYPE

%token LBRACKET RBRACKET
%token LBRACE RBRACE LPAREN RPAREN
%token DOT COMMA COLON

%token <intval> INT_LITERAL
%token <floatval> FLOAT_LITERAL
%token <str> IDENTIFIER STRING_LITERAL

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE_VERIFY OTHERWISE

%%

program
    : mission_block SUCCESS
    ;

mission_block
    : MISSION IDENTIFIER LAUNCH block
    ;

block
    : LBRACE statement_list RBRACE
    ;

statement_list
    : statement_list statement
    |
    ;

statement
    : declaration
    | expression DOT
    | control_statement
    | loop_statement
    | switch_statement
    | module_definition
    | wait_statement
    | STAGE_SEP DOT
    | COAST DOT
    | BACK DOT
    | ABORT DOT
    ;

declaration
    : TELEMETRY type IDENTIFIER DOT
    | TELEMETRY type IDENTIFIER ASSIGN expression DOT
    | LIMIT type IDENTIFIER ASSIGN expression DOT
    | ALIAS type IDENTIFIER DOT
    | FLEET type IDENTIFIER DOT
    | MODE IDENTIFIER LBRACE mode_body RBRACE
    | TELEMETRY type IDENTIFIER LBRACKET INT_LITERAL RBRACKET DOT
    | TELEMETRY type IDENTIFIER LBRACKET INT_LITERAL RBRACKET ASSIGN expression DOT
    ;

mode_body
    : mode_body mode_entry
    |
    ;

mode_entry
    : TRAJECTORY INT_LITERAL DOT
    | FALLBACK INT_LITERAL DOT
    ;

type
    : COUNT
    | REAL
    | PRECISE
    | FLAG
    | SYMBOL
    | VOIDSPACE
    ;

control_statement
    : VERIFY LPAREN expression RPAREN block %prec LOWER_THAN_ELSE
    | VERIFY LPAREN expression RPAREN block ELSE_VERIFY LPAREN expression RPAREN block
    | VERIFY LPAREN expression RPAREN block OTHERWISE block
    ;

loop_statement
    : ORBIT LPAREN expression RPAREN block
    | ORBIT WHILE LPAREN expression RPAREN block
    | ORBIT TIMES LPAREN expression COLON expression COLON expression RPAREN block
    ;

switch_statement
    : SCENARIO LPAREN expression RPAREN LBRACE case_list RBRACE
    ;

case_list
    : case_list case_entry
    |
    ;

case_entry
    : TRAJECTORY INT_LITERAL COLON statement_list
    | FALLBACK COLON statement_list
    ;

wait_statement
    : WAIT INT_LITERAL TICK DOT
    ;

module_definition
    : MODULE IDENTIFIER inheritance LBRACE class_body RBRACE
    ;

inheritance
    : EXTENDS IDENTIFIER
    |
    ;

class_body
    : class_body class_member
    |
    ;

class_member
    : declaration
    | PUBLIC declaration
    | PRIVATE declaration
    | function_definition
    ;

function_definition
    : COMMAND IDENTIFIER LPAREN parameter_list RPAREN return_type type block
    ;

return_type
    : COLON
    | RETTYPE
    ;

parameter_list
    : parameter_list COMMA parameter
    | parameter
    |
    ;

parameter
    : type IDENTIFIER
    ;

expression
    : assignment_expression
    ;

assignment_expression
    : logical_expression
    | logical_expression ASSIGN assignment_expression
    ;

logical_expression
    : logical_expression OR equality_expression
    | logical_expression AND equality_expression
    | logical_expression XOR equality_expression
    | equality_expression
    ;

equality_expression
    : equality_expression EQ relational_expression
    | equality_expression NEQ relational_expression
    | relational_expression
    ;

relational_expression
    : relational_expression LT additive_expression
    | relational_expression GT additive_expression
    | relational_expression LE additive_expression
    | relational_expression GE additive_expression
    | additive_expression
    ;

additive_expression
    : additive_expression ADD multiplicative_expression
    | additive_expression MINUS multiplicative_expression
    | multiplicative_expression
    ;

multiplicative_expression
    : multiplicative_expression MUL power_expression
    | multiplicative_expression DIVIDE power_expression
    | multiplicative_expression MOD power_expression
    | power_expression
    ;

power_expression
    : power_expression POWER unary_expression
    | unary_expression
    ;

unary_expression
    : NOT unary_expression
    | postfix_expression
    ;

postfix_expression
    : primary_expression
    | postfix_expression LPAREN argument_list RPAREN
    | postfix_expression LBRACKET expression RBRACKET
    ;

primary_expression
    : IDENTIFIER
    | THIS
    | INT_LITERAL
    | FLOAT_LITERAL
    | STRING_LITERAL
    | DEPLOY IDENTIFIER LPAREN RPAREN
    | builtin_call
    | LPAREN expression RPAREN
    ;

builtin_call
    : BROADCAST LPAREN argument_list RPAREN
    | RECEIVE LPAREN RPAREN
    | ALARM LPAREN argument_list RPAREN
    | ROOT LPAREN expression RPAREN
    | FLR LPAREN expression RPAREN
    | CEIL LPAREN expression RPAREN
    | ABS LPAREN expression RPAREN
    | LOGARITHM LPAREN expression RPAREN
    | SINE LPAREN expression RPAREN
    | COSINE LPAREN expression RPAREN
    | TAN LPAREN expression RPAREN
    | ASINE LPAREN expression RPAREN
    | ACOSINE LPAREN expression RPAREN
    | ATAN LPAREN expression RPAREN
    | PRIME LPAREN expression RPAREN
    ;

argument_list
    : argument_list COMMA expression
    | expression
    |
    ;


%%

void yyerror(const char *s) {
    printf("SYNTAX ERROR at line %d: %s\n", yylineno, s);
}

int main(int argc, char** argv) {
    if (argc > 1) {
        FILE* file = fopen(argv[1], "r");
        if (!file) {
            printf("Cannot open file\n");
            return 1;
        }
        yyin = file;
    }
    return yyparse();
}
