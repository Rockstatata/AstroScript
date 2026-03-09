%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "symbol_table.h"
#include "tac.h"

int yylex();
void yyerror(const char *s);
extern FILE *yyin;
extern int yylineno;

SymbolTable symbolTable;
TACGenerator tacGenerator;

struct ExprAttr {
    std::string place;
    double value;
    bool isConst;
    bool isIdentifier;
};

struct IfContext {
    std::string elseLabel;
    std::string endLabel;
};

struct LoopContext {
    std::string startLabel;
    std::string endLabel;
    std::string iteratorPlace;
    bool hasIterator;
};

struct SwitchContext {
    std::string exprPlace;
    std::string endLabel;
    std::string nextLabel;
    bool hasDefault;
};

std::vector<IfContext> ifStack;
std::vector<LoopContext> loopStack;
std::vector<SwitchContext> switchStack;
std::vector<std::string> currentFunctionParams;
std::string currentFunctionName;
std::unordered_map<std::string, int> functionArity;

bool isParameterName(const std::string& name) {
    for (const std::string& param : currentFunctionParams) {
        if (param == name) {
            return true;
        }
    }
    return false;
}

bool isDeclaredName(const std::string& name) {
    return symbolTable.exists(name) || isParameterName(name);
}

std::string toText(double value) {
    std::ostringstream stream;
    stream << value;
    return stream.str();
}

ExprAttr* makeExpr(const std::string& place, double value, bool isConst, bool isIdentifier = false) {
    return new ExprAttr{place, value, isConst, isIdentifier};
}

ExprAttr* makeNumericExpr(double value) {
    return makeExpr(toText(value), value, true);
}
%}

%code requires {
    struct ExprAttr;
}


%union {
    int intval;
    double floatval;
    char* str;
    ExprAttr* expr;
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
%type <expr> expression
%type <intval> argument_list argument_list_opt
%type <intval> parameter_list parameter_list_opt

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
    | input_stmt
    | expression DOT
      {
          delete $1;
      }
    | control_statement
    | loop_statement
    | switch_statement
    | module_definition
    | wait_statement
    | STAGE_SEP DOT
    | COAST DOT
    | return_statement
    | ABORT DOT
    ;

declaration
    : TELEMETRY type IDENTIFIER DOT
      {
          if (symbolTable.exists($3)) {
              printf("Semantic Error: variable %s already declared\n", $3);
          } else {
              symbolTable.insert($3, $2);
              tacGenerator.emitDeclare($2, $3);
          }
      }
    | TELEMETRY type IDENTIFIER ASSIGN expression DOT
      {
          if (symbolTable.exists($3)) {
              printf("Semantic Error: variable %s already declared\n", $3);
          } else {
              symbolTable.insert($3, $2);
              tacGenerator.emitDeclare($2, $3);
              tacGenerator.emitAssign($3, $5->place);
          }
          delete $5;
      }
    | LIMIT type IDENTIFIER ASSIGN expression DOT
      {
          if (symbolTable.exists($3)) {
              printf("Semantic Error: variable %s already declared\n", $3);
          } else {
              symbolTable.insert($3, $2);
              tacGenerator.emitDeclare($2, $3);
              tacGenerator.emitAssign($3, $5->place);
          }
          delete $5;
      }
    | ALIAS type IDENTIFIER DOT
    | FLEET type IDENTIFIER DOT
    | MODE IDENTIFIER LBRACE mode_body RBRACE
    | TELEMETRY type IDENTIFIER LBRACKET INT_LITERAL RBRACKET DOT
      {
          if (symbolTable.exists($3)) {
              printf("Semantic Error: variable %s already declared\n", $3);
          } else {
              symbolTable.insert($3, std::string($2) + "[]");
              tacGenerator.emitArrayDecl($2, $3, toText(static_cast<double>($5)));
          }
      }
    | TELEMETRY type IDENTIFIER LBRACKET INT_LITERAL RBRACKET ASSIGN expression DOT
      {
          if (symbolTable.exists($3)) {
              printf("Semantic Error: variable %s already declared\n", $3);
          } else {
              symbolTable.insert($3, std::string($2) + "[]");
              tacGenerator.emitArrayDecl($2, $3, toText(static_cast<double>($5)));
              tacGenerator.emitArrayStore($3, "0", $8->place);
          }
          delete $8;
      }
    ;

assignment
    : IDENTIFIER ASSIGN expression DOT
      {
          if (!isDeclaredName($1)) {
              printf("Semantic Error: variable %s not declared\n", $1);
          } else {
              tacGenerator.emitAssign($1, $3->place);
          }
          delete $3;
      }
    | IDENTIFIER LBRACKET expression RBRACKET ASSIGN expression DOT
      {
          if (!isDeclaredName($1)) {
              printf("Semantic Error: array %s not declared\n", $1);
          } else {
              tacGenerator.emitArrayStore($1, $3->place, $6->place);
          }
          delete $3;
          delete $6;
      }
    ;

print_stmt
    : TRANSMIT expression DOT
      {
          tacGenerator.emitPrint($2->place);
          delete $2;
      }
    ;

input_stmt
    : RECEIVE IDENTIFIER DOT
      {
          if (!isDeclaredName($2)) {
              printf("Semantic Error: variable %s not declared\n", $2);
          } else {
              tacGenerator.emitInput($2);
          }
      }
    | RECEIVE IDENTIFIER LBRACKET expression RBRACKET DOT
      {
          if (!isDeclaredName($2)) {
              printf("Semantic Error: array %s not declared\n", $2);
          } else {
              std::string temp = tacGenerator.newTemp();
              tacGenerator.emitInput(temp);
              tacGenerator.emitArrayStore($2, $4->place, temp);
          }
          delete $4;
      }
    ;

return_statement
    : BACK DOT
      {
          tacGenerator.emitFunctionReturn();
      }
    | BACK expression DOT
      {
          tacGenerator.emitFunctionReturn($2->place);
          delete $2;
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
    : VERIFY LPAREN expression RPAREN
      {
          IfContext ctx{tacGenerator.newLabel(), tacGenerator.newLabel()};
          tacGenerator.emitIfFalseGoto($3->place, ctx.elseLabel);
          ifStack.push_back(ctx);
          delete $3;
      }
      block
      {
          IfContext& ctx = ifStack.back();
          tacGenerator.emitGoto(ctx.endLabel);
          tacGenerator.emitLabel(ctx.elseLabel);
      }
      else_verify_chain otherwise_clause
      {
          IfContext ctx = ifStack.back();
          ifStack.pop_back();
          tacGenerator.emitLabel(ctx.endLabel);
      }
    ;

else_verify_chain
    : ELSE_VERIFY LPAREN expression RPAREN
      {
          IfContext& ctx = ifStack.back();
          std::string nextLabel = tacGenerator.newLabel();
          tacGenerator.emitIfFalseGoto($3->place, nextLabel);
          ctx.elseLabel = nextLabel;
          delete $3;
      }
      block
      {
          IfContext& ctx = ifStack.back();
          tacGenerator.emitGoto(ctx.endLabel);
          tacGenerator.emitLabel(ctx.elseLabel);
      }
      else_verify_chain
    | %prec LOWER_THAN_ELSE
    ;

otherwise_clause
    : OTHERWISE block
    |
    ;

loop_statement
    : ORBIT LPAREN
      {
          LoopContext ctx{tacGenerator.newLabel(), tacGenerator.newLabel(), "", false};
          loopStack.push_back(ctx);
          tacGenerator.emitLabel(ctx.startLabel);
      }
      expression RPAREN
      {
          LoopContext& ctx = loopStack.back();
          tacGenerator.emitIfFalseGoto($4->place, ctx.endLabel);
          delete $4;
      }
      block
      {
          LoopContext ctx = loopStack.back();
          loopStack.pop_back();
          tacGenerator.emitGoto(ctx.startLabel);
          tacGenerator.emitLabel(ctx.endLabel);
      }
    | ORBIT WHILE LPAREN
      {
          LoopContext ctx{tacGenerator.newLabel(), tacGenerator.newLabel(), "", false};
          loopStack.push_back(ctx);
          tacGenerator.emitLabel(ctx.startLabel);
      }
      expression RPAREN
      {
          LoopContext& ctx = loopStack.back();
          tacGenerator.emitIfFalseGoto($5->place, ctx.endLabel);
          delete $5;
      }
      block
      {
          LoopContext ctx = loopStack.back();
          loopStack.pop_back();
          tacGenerator.emitGoto(ctx.startLabel);
          tacGenerator.emitLabel(ctx.endLabel);
      }
    | ORBIT TIMES LPAREN expression COLON expression COLON expression RPAREN
      {
          LoopContext ctx{tacGenerator.newLabel(), tacGenerator.newLabel(), $4->place, true};
          loopStack.push_back(ctx);
          tacGenerator.emitAssign($4->place, $6->place);
          tacGenerator.emitLabel(ctx.startLabel);
          std::string conditionTemp = tacGenerator.emitBinary("<", $4->place, $8->place);
          tacGenerator.emitIfFalseGoto(conditionTemp, ctx.endLabel);
          delete $4;
          delete $6;
          delete $8;
      }
      block
      {
          LoopContext ctx = loopStack.back();
          loopStack.pop_back();
          if (ctx.hasIterator) {
              std::string incrementTemp = tacGenerator.emitBinary("+", ctx.iteratorPlace, "1");
              tacGenerator.emitAssign(ctx.iteratorPlace, incrementTemp);
          }
          tacGenerator.emitGoto(ctx.startLabel);
          tacGenerator.emitLabel(ctx.endLabel);
      }
    ;

switch_statement
    : SCENARIO LPAREN expression RPAREN
      {
          SwitchContext ctx{$3->place, tacGenerator.newLabel(), "", false};
          switchStack.push_back(ctx);
          delete $3;
      }
      LBRACE case_list RBRACE
      {
          SwitchContext ctx = switchStack.back();
          switchStack.pop_back();
          if (!ctx.nextLabel.empty()) {
              tacGenerator.emitLabel(ctx.nextLabel);
          }
          tacGenerator.emitLabel(ctx.endLabel);
      }
    ;

case_list
    : case_list case_entry
    |
    ;

case_entry
    : TRAJECTORY INT_LITERAL COLON
      {
          SwitchContext& ctx = switchStack.back();
          if (!ctx.nextLabel.empty()) {
              tacGenerator.emitLabel(ctx.nextLabel);
          }
          std::string compareTemp = tacGenerator.emitBinary("==", ctx.exprPlace, toText(static_cast<double>($2)));
          ctx.nextLabel = tacGenerator.newLabel();
          tacGenerator.emitIfFalseGoto(compareTemp, ctx.nextLabel);
      }
      statement_list
      {
          SwitchContext& ctx = switchStack.back();
          tacGenerator.emitGoto(ctx.endLabel);
      }
    | FALLBACK COLON
      {
          SwitchContext& ctx = switchStack.back();
          if (!ctx.nextLabel.empty()) {
              tacGenerator.emitLabel(ctx.nextLabel);
              ctx.nextLabel.clear();
          }
          ctx.hasDefault = true;
      }
      statement_list
    ;

wait_statement
    : WAIT INT_LITERAL TICK DOT
      {
          tacGenerator.emit("wait", toText(static_cast<double>($2)), "", "");
      }
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
    : COMMAND IDENTIFIER LPAREN
      {
          currentFunctionName = $2;
          currentFunctionParams.clear();
      }
      parameter_list_opt RPAREN return_type type
      {
          functionArity[currentFunctionName] = $5;
          tacGenerator.emitFunctionBegin(currentFunctionName);
          for (const std::string& parameterName : currentFunctionParams) {
              tacGenerator.emitParamDef(parameterName);
          }
      }
      block
      {
          tacGenerator.emitFunctionReturn();
          tacGenerator.emitFunctionEnd(currentFunctionName);
          currentFunctionParams.clear();
      }
    ;

return_type
    : COLON
    | RETTYPE
    ;

parameter_list_opt
    : parameter_list { $$ = $1; }
    | { $$ = 0; }
    ;

parameter_list
    : parameter_list COMMA parameter
      {
          $$ = $1 + 1;
      }
    | parameter
      {
          $$ = 1;
      }
    ;

parameter
    : type IDENTIFIER
      {
          currentFunctionParams.push_back($2);
      }
    ;

argument_list_opt
    : argument_list { $$ = $1; }
    | { $$ = 0; }
    ;

argument_list
    : expression
      {
          tacGenerator.emitParam($1->place);
          delete $1;
          $$ = 1;
      }
    | argument_list COMMA expression
      {
          tacGenerator.emitParam($3->place);
          delete $3;
          $$ = $1 + 1;
      }
    ;

expression
        : expression LT expression
            {
                std::string place = tacGenerator.emitBinary("<", $1->place, $3->place);
                $$ = makeExpr(place, ($1->value < $3->value) ? 1.0 : 0.0, $1->isConst && $3->isConst);
                delete $1;
                delete $3;
            }
        | expression GT expression
            {
                std::string place = tacGenerator.emitBinary(">", $1->place, $3->place);
                $$ = makeExpr(place, ($1->value > $3->value) ? 1.0 : 0.0, $1->isConst && $3->isConst);
                delete $1;
                delete $3;
            }
        | expression LE expression
            {
                std::string place = tacGenerator.emitBinary("<=", $1->place, $3->place);
                $$ = makeExpr(place, ($1->value <= $3->value) ? 1.0 : 0.0, $1->isConst && $3->isConst);
                delete $1;
                delete $3;
            }
        | expression GE expression
            {
                std::string place = tacGenerator.emitBinary(">=", $1->place, $3->place);
                $$ = makeExpr(place, ($1->value >= $3->value) ? 1.0 : 0.0, $1->isConst && $3->isConst);
                delete $1;
                delete $3;
            }
        | expression EQ expression
            {
                std::string place = tacGenerator.emitBinary("==", $1->place, $3->place);
                $$ = makeExpr(place, ($1->value == $3->value) ? 1.0 : 0.0, $1->isConst && $3->isConst);
                delete $1;
                delete $3;
            }
        | expression NEQ expression
            {
                std::string place = tacGenerator.emitBinary("!=", $1->place, $3->place);
                $$ = makeExpr(place, ($1->value != $3->value) ? 1.0 : 0.0, $1->isConst && $3->isConst);
                delete $1;
                delete $3;
            }
        | expression ADD expression
      {
          std::string place = tacGenerator.emitBinary("+", $1->place, $3->place);
                    $$ = makeExpr(place, 0.0, false);
          delete $1;
          delete $3;
      }
    | expression MINUS expression
      {
          std::string place = tacGenerator.emitBinary("-", $1->place, $3->place);
                    $$ = makeExpr(place, 0.0, false);
          delete $1;
          delete $3;
      }
    | expression MUL expression
      {
          std::string place = tacGenerator.emitBinary("*", $1->place, $3->place);
                    $$ = makeExpr(place, 0.0, false);
          delete $1;
          delete $3;
      }
    | expression DIVIDE expression
      {
          std::string place = tacGenerator.emitBinary("/", $1->place, $3->place);
                    $$ = makeExpr(place, 0.0, false);
          delete $1;
          delete $3;
      }
    | INT_LITERAL
      {
          $$ = makeNumericExpr(static_cast<double>($1));
      }
    | FLOAT_LITERAL
      {
          $$ = makeNumericExpr($1);
      }
        | STRING_LITERAL
            {
                    $$ = makeExpr($1, 0.0, true);
            }
    | IDENTIFIER
      {
          if (!isDeclaredName($1)) {
              printf("Semantic Error: variable %s not declared\n", $1);
              $$ = makeExpr($1, 0.0, false);
          } else {
                            $$ = makeExpr($1, 0.0, false, true);
          }
      }
        | IDENTIFIER LBRACKET expression RBRACKET
            {
                      if (!isDeclaredName($1)) {
                            printf("Semantic Error: array %s not declared\n", $1);
                            $$ = makeExpr("0", 0.0, false);
                    } else {
                            std::string temp = tacGenerator.emitArrayLoad($1, $3->place);
                            $$ = makeExpr(temp, 0.0, false);
                    }
                    delete $3;
            }
        | IDENTIFIER LPAREN argument_list_opt RPAREN
            {
                    std::string callTemp = tacGenerator.emitCall($1, $3);
                    $$ = makeExpr(callTemp, 0.0, false);
            }
    | LPAREN expression RPAREN
      {
          $$ = $2;
      }
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
    int parseStatus = yyparse();
    if (parseStatus == 0) {
        tacGenerator.optimize();
        tacGenerator.printCode("Optimized Three Address Code");
        tacGenerator.execute();
        symbolTable.printTable();
    }
    return parseStatus;
}
