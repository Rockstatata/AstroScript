%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "symbol_table.h"

int yylex();
void yyerror(const char *s);
extern FILE *yyin;
extern int yylineno;

SymbolTable symbolTable;
%}


%union {
    int intval;
    double floatval;
    char* str;
}

%token MISSION LAUNCH SUCCESS ABORT
%token MODULE DEPLOY EXTENDS PUBLIC PRIVATE THIS
%token TELEMETRY LIMIT ALIAS FLEET MODE
%token COMMAND BACK TRANSMIT
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

%type <str> type
%type <floatval> expression

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE_VERIFY OTHERWISE
%left LT GT LE GE EQ NEQ
%left ADD MINUS
%left MUL DIVIDE

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
    | assignment
    | print_stmt
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
      {
          if (symbolTable.exists($3)) {
              printf("Semantic Error: variable %s already declared\n", $3);
          } else {
              symbolTable.insert($3, $2);
          }
      }
    | TELEMETRY type IDENTIFIER ASSIGN expression DOT
      {
          if (symbolTable.exists($3)) {
              printf("Semantic Error: variable %s already declared\n", $3);
          } else {
              symbolTable.insert($3, $2);
              symbolTable.setValue($3, $5);
          }
      }
    | LIMIT type IDENTIFIER ASSIGN expression DOT
      {
          if (symbolTable.exists($3)) {
              printf("Semantic Error: variable %s already declared\n", $3);
          } else {
              symbolTable.insert($3, $2);
              symbolTable.setValue($3, $5);
          }
      }
    | ALIAS type IDENTIFIER DOT
    | FLEET type IDENTIFIER DOT
    | MODE IDENTIFIER LBRACE mode_body RBRACE
    | TELEMETRY type IDENTIFIER LBRACKET INT_LITERAL RBRACKET DOT
    | TELEMETRY type IDENTIFIER LBRACKET INT_LITERAL RBRACKET ASSIGN expression DOT
    ;

assignment
    : IDENTIFIER ASSIGN expression DOT
      {
          if (!symbolTable.exists($1)) {
              printf("Semantic Error: variable %s not declared\n", $1);
          } else {
              symbolTable.setValue($1, $3);
          }
      }
    ;

print_stmt
    : TRANSMIT IDENTIFIER DOT
      {
          if (!symbolTable.exists($2)) {
              printf("Semantic Error: variable %s not declared\n", $2);
          } else {
              std::cout << "PRINT: " << symbolTable.getValue($2) << std::endl;
          }
      }
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
    : COUNT { $$ = strdup("COUNT"); }
    | REAL { $$ = strdup("REAL"); }
    | PRECISE { $$ = strdup("PRECISE"); }
    | FLAG { $$ = strdup("FLAG"); }
    | SYMBOL { $$ = strdup("SYMBOL"); }
    | VOIDSPACE { $$ = strdup("VOIDSPACE"); }
    ;

control_statement
    : VERIFY LPAREN expression RPAREN block else_verify_chain otherwise_clause
    ;

else_verify_chain
    : ELSE_VERIFY LPAREN expression RPAREN block else_verify_chain
    | %prec LOWER_THAN_ELSE
    ;

otherwise_clause
    : OTHERWISE block
    |
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
        : expression LT expression
            { $$ = ($1 < $3) ? 1.0 : 0.0; }
        | expression GT expression
            { $$ = ($1 > $3) ? 1.0 : 0.0; }
        | expression LE expression
            { $$ = ($1 <= $3) ? 1.0 : 0.0; }
        | expression GE expression
            { $$ = ($1 >= $3) ? 1.0 : 0.0; }
        | expression EQ expression
            { $$ = ($1 == $3) ? 1.0 : 0.0; }
        | expression NEQ expression
            { $$ = ($1 != $3) ? 1.0 : 0.0; }
        | expression ADD expression
      { $$ = $1 + $3; }
    | expression MINUS expression
      { $$ = $1 - $3; }
    | expression MUL expression
      { $$ = $1 * $3; }
    | expression DIVIDE expression
      { $$ = $1 / $3; }
    | INT_LITERAL
      { $$ = static_cast<double>($1); }
    | IDENTIFIER
      {
          if (!symbolTable.exists($1)) {
              printf("Semantic Error: variable %s not declared\n", $1);
              $$ = 0.0;
          } else {
              $$ = symbolTable.getValue($1);
          }
      }
    | LPAREN expression RPAREN
      { $$ = $2; }
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
