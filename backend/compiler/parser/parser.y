%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../semantic/symbol_table.h"
#include "../ir/tac.h"

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
    std::string typeName;
};

struct IfContext {
    std::string elseLabel;
    std::string endLabel;
};

struct LoopContext {
    std::string startLabel;
    std::string endLabel;
    std::string continueLabel;
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
std::unordered_map<std::string, std::string> currentFunctionParamTypes;
std::string currentFunctionName;
std::string currentFunctionReturnType = "VOIDSPACE";
bool currentFunctionInModule = false;
std::unordered_map<std::string, int> functionArity;
std::unordered_map<std::string, std::unordered_map<std::string, std::string>> functionParameterTypes;
std::unordered_map<std::string, std::string> functionReturnTypes;
std::unordered_map<std::string, std::string> typeAliases;
std::unordered_set<std::string> constantNames;
int moduleDepth = 0;
bool currentFunctionSawValueReturn = false;

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

std::string baseType(const std::string& typeName) {
    if (typeName.size() > 2 && typeName.substr(typeName.size() - 2) == "[]") {
        return typeName.substr(0, typeName.size() - 2);
    }
    return typeName;
}

bool isNumericType(const std::string& typeName) {
    const std::string type = baseType(typeName);
    return type == "COUNT" || type == "REAL" || type == "PRECISE" || type == "FLAG";
}

bool isStringType(const std::string& typeName) {
    return baseType(typeName) == "SYMBOL";
}

bool isBoolType(const std::string& typeName) {
    return baseType(typeName) == "FLAG";
}

std::string numericResultType(const std::string& leftType, const std::string& rightType) {
    const std::string left = baseType(leftType);
    const std::string right = baseType(rightType);
    if (left == "PRECISE" || right == "PRECISE") {
        return "PRECISE";
    }
    if (left == "REAL" || right == "REAL") {
        return "REAL";
    }
    return "COUNT";
}

std::string declaredTypeOf(const std::string& name) {
    auto paramIt = currentFunctionParamTypes.find(name);
    if (paramIt != currentFunctionParamTypes.end()) {
        return paramIt->second;
    }

    if (symbolTable.exists(name)) {
        return symbolTable.getType(name);
    }

    return "UNKNOWN";
}

bool areTypesCompatible(const std::string& targetType, const std::string& sourceType) {
    const std::string target = baseType(targetType);
    const std::string source = baseType(sourceType);

    if (target == "UNKNOWN" || source == "UNKNOWN") {
        return true;
    }

    if (target == source) {
        return true;
    }

    if (isNumericType(target) && isNumericType(source)) {
        return true;
    }

    return false;
}

std::string toText(double value) {
    std::ostringstream stream;
    stream << value;
    return stream.str();
}

ExprAttr* makeExpr(const std::string& place, double value, bool isConst, bool isIdentifier = false, const std::string& typeName = "UNKNOWN") {
    return new ExprAttr{place, value, isConst, isIdentifier, typeName};
}

ExprAttr* makeNumericExpr(double value) {
    return makeExpr(toText(value), value, true, false, "COUNT");
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
%left OR XOR
%left AND
%right NOT
%left LT GT LE GE EQ NEQ
%left ADD MINUS
%left MUL DIVIDE MOD
%right POWER

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
    | function_definition
    | print_stmt
    | input_stmt
    | deploy_statement
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
      {
          if (!loopStack.empty()) {
              tacGenerator.emitGoto(loopStack.back().endLabel);
          } else if (!switchStack.empty()) {
              tacGenerator.emitGoto(switchStack.back().endLabel);
          } else {
              printf("Semantic Error at line %d: stage_sep used outside loop/scenario\n", yylineno);
          }
      }
    | COAST DOT
      {
          if (!loopStack.empty()) {
              tacGenerator.emitGoto(loopStack.back().continueLabel);
          } else {
              printf("Semantic Error at line %d: coast used outside loop\n", yylineno);
          }
      }
    | return_statement
    | ABORT DOT
    ;

declaration
    : TELEMETRY type IDENTIFIER DOT
      {
          if (symbolTable.exists($3)) {
              printf("Semantic Error at line %d: variable %s already declared\n", yylineno, $3);
          } else {
              symbolTable.insert($3, $2);
              tacGenerator.emitDeclare($2, $3);
          }
      }
    | TELEMETRY type IDENTIFIER ASSIGN expression DOT
      {
          if (symbolTable.exists($3)) {
              printf("Semantic Error at line %d: variable %s already declared\n", yylineno, $3);
          } else {
              symbolTable.insert($3, $2);
              tacGenerator.emitDeclare($2, $3);
              if (!areTypesCompatible($2, $5->typeName)) {
                  printf("Semantic Error at line %d: cannot initialize %s (%s) with %s\n", yylineno, $3, $2, $5->typeName.c_str());
              }
              tacGenerator.emitAssign($3, $5->place);
          }
          delete $5;
      }
    | LIMIT type IDENTIFIER ASSIGN expression DOT
      {
          if (symbolTable.exists($3)) {
              printf("Semantic Error at line %d: variable %s already declared\n", yylineno, $3);
          } else {
              symbolTable.insert($3, $2);
              constantNames.insert($3);
              tacGenerator.emitDeclare($2, $3);
              if (!areTypesCompatible($2, $5->typeName)) {
                  printf("Semantic Error at line %d: cannot initialize constant %s (%s) with %s\n", yylineno, $3, $2, $5->typeName.c_str());
              }
              tacGenerator.emitAssign($3, $5->place);
          }
          delete $5;
      }
    | ALIAS type IDENTIFIER DOT
      {
          typeAliases[$3] = $2;
      }
    | FLEET type IDENTIFIER DOT
      {
          if (symbolTable.exists($3)) {
              printf("Semantic Error at line %d: variable %s already declared\n", yylineno, $3);
          } else {
              symbolTable.insert($3, std::string($2) + "[]");
              tacGenerator.emitArrayDecl($2, $3, "0");
          }
      }
    | MODE IDENTIFIER LBRACE mode_body RBRACE
    | TELEMETRY type IDENTIFIER LBRACKET INT_LITERAL RBRACKET DOT
      {
          if (symbolTable.exists($3)) {
              printf("Semantic Error at line %d: variable %s already declared\n", yylineno, $3);
          } else {
              symbolTable.insert($3, std::string($2) + "[]");
              tacGenerator.emitArrayDecl($2, $3, toText(static_cast<double>($5)));
          }
      }
    | TELEMETRY type IDENTIFIER LBRACKET INT_LITERAL RBRACKET ASSIGN expression DOT
      {
          if (symbolTable.exists($3)) {
              printf("Semantic Error at line %d: variable %s already declared\n", yylineno, $3);
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
              printf("Semantic Error at line %d: variable %s not declared\n", yylineno, $1);
          } else if (constantNames.find($1) != constantNames.end()) {
              printf("Semantic Error at line %d: cannot assign to constant %s\n", yylineno, $1);
          } else {
              std::string destinationType = declaredTypeOf($1);
              if (!areTypesCompatible(destinationType, $3->typeName)) {
                  printf("Semantic Error at line %d: type mismatch assigning %s to %s (%s)\n", yylineno, $3->typeName.c_str(), $1, destinationType.c_str());
              }
              tacGenerator.emitAssign($1, $3->place);
          }
          delete $3;
      }
    | IDENTIFIER LBRACKET expression RBRACKET ASSIGN expression DOT
      {
          if (!isDeclaredName($1)) {
              printf("Semantic Error at line %d: array %s not declared\n", yylineno, $1);
          } else {
              std::string arrayType = declaredTypeOf($1);
              std::string elementType = baseType(arrayType);
              if (!areTypesCompatible(elementType, $6->typeName)) {
                  printf("Semantic Error at line %d: type mismatch storing %s into %s[%s]\n", yylineno, $6->typeName.c_str(), $1, $3->place.c_str());
              }
              tacGenerator.emitArrayStore($1, $3->place, $6->place);
          }
          delete $3;
          delete $6;
      }
    ;

deploy_statement
    : DEPLOY IDENTIFIER IDENTIFIER DOT
      {
          if (symbolTable.exists($3)) {
              printf("Semantic Error at line %d: variable %s already declared\n", yylineno, $3);
          } else {
              symbolTable.insert($3, std::string("MODULE:") + $2);
              tacGenerator.emitDeclare("MODULE", $3);
          }
      }
    | DEPLOY IDENTIFIER DOT
      {
          if (symbolTable.exists($2)) {
              printf("Semantic Error at line %d: variable %s already declared\n", yylineno, $2);
          } else {
              symbolTable.insert($2, "MODULE");
              tacGenerator.emitDeclare("MODULE", $2);
          }
      }
    ;

print_stmt
    : TRANSMIT expression DOT
      {
          tacGenerator.emitPrint($2->place);
          delete $2;
      }
        | BROADCAST expression DOT
            {
                    tacGenerator.emitPrint($2->place);
                    delete $2;
            }
        | ALARM expression DOT
            {
                    tacGenerator.emitPrint($2->place);
                    delete $2;
            }
    ;

input_stmt
    : RECEIVE IDENTIFIER DOT
      {
          if (!isDeclaredName($2)) {
              printf("Semantic Error at line %d: variable %s not declared\n", yylineno, $2);
          } else if (constantNames.find($2) != constantNames.end()) {
              printf("Semantic Error at line %d: cannot receive input into constant %s\n", yylineno, $2);
          } else {
              tacGenerator.emitInput($2);
          }
      }
    | RECEIVE IDENTIFIER LBRACKET expression RBRACKET DOT
      {
          if (!isDeclaredName($2)) {
              printf("Semantic Error at line %d: array %s not declared\n", yylineno, $2);
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
                    if (currentFunctionReturnType != "VOIDSPACE") {
                            printf("Semantic Error at line %d: function %s must return a value of type %s\n", yylineno, currentFunctionName.c_str(), currentFunctionReturnType.c_str());
                    }
          tacGenerator.emitFunctionReturn();
      }
    | BACK expression DOT
      {
          if (currentFunctionReturnType == "VOIDSPACE") {
              printf("Semantic Error at line %d: voidspace function cannot return a value\n", yylineno);
          } else if (!areTypesCompatible(currentFunctionReturnType, $2->typeName)) {
              printf("Semantic Error at line %d: return type mismatch, expected %s but got %s\n", yylineno, currentFunctionReturnType.c_str(), $2->typeName.c_str());
          }
          currentFunctionSawValueReturn = true;
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
    | IDENTIFIER
      {
          auto aliasIt = typeAliases.find($1);
          if (aliasIt == typeAliases.end()) {
              printf("Semantic Error at line %d: unknown type alias %s\n", yylineno, $1);
              $$ = strdup("COUNT");
          } else {
              $$ = strdup(aliasIt->second.c_str());
          }
      }
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
          LoopContext ctx{tacGenerator.newLabel(), tacGenerator.newLabel(), "", "", false};
          ctx.continueLabel = ctx.startLabel;
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
                    LoopContext ctx{tacGenerator.newLabel(), tacGenerator.newLabel(), "", "", false};
                    ctx.continueLabel = ctx.startLabel;
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
                    LoopContext ctx{tacGenerator.newLabel(), tacGenerator.newLabel(), tacGenerator.newLabel(), $4->place, true};
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
              tacGenerator.emitLabel(ctx.continueLabel);
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
    : MODULE IDENTIFIER
      {
          ++moduleDepth;
      }
      inheritance LBRACE class_body RBRACE
      {
          --moduleDepth;
      }
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
          currentFunctionParamTypes.clear();
          currentFunctionInModule = moduleDepth > 0;
          currentFunctionSawValueReturn = false;
      }
      parameter_list_opt RPAREN return_type type
      {
          functionArity[currentFunctionName] = $5;
          functionParameterTypes[currentFunctionName] = currentFunctionParamTypes;
          functionReturnTypes[currentFunctionName] = $8;
          currentFunctionReturnType = $8;
          if (currentFunctionInModule) {
              currentFunctionParamTypes["this"] = "MODULE";
          }
          tacGenerator.emitFunctionBegin(currentFunctionName);
          for (const std::string& parameterName : currentFunctionParams) {
              tacGenerator.emitParamDef(parameterName);
          }
      }
      block
      {
          if (currentFunctionReturnType != "VOIDSPACE" && !currentFunctionSawValueReturn) {
              // Keep execution flowing but flag implicit missing return in non-voidspace functions.
              printf("Semantic Error at line %d: function %s may exit without explicit back value\n", yylineno, currentFunctionName.c_str());
          }
          tacGenerator.emitFunctionReturn();
          tacGenerator.emitFunctionEnd(currentFunctionName);
          currentFunctionParams.clear();
          currentFunctionParamTypes.clear();
          currentFunctionReturnType = "VOIDSPACE";
          currentFunctionInModule = false;
                    currentFunctionSawValueReturn = false;
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
                    currentFunctionParamTypes[$2] = $1;
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
                if (!areTypesCompatible($1->typeName, $3->typeName)) {
                    printf("Semantic Error at line %d: incompatible types for comparison (%s, %s)\n", yylineno, $1->typeName.c_str(), $3->typeName.c_str());
                }
                $$ = makeExpr(place, ($1->value < $3->value) ? 1.0 : 0.0, $1->isConst && $3->isConst, false, "FLAG");
                delete $1;
                delete $3;
            }
        | expression GT expression
            {
                std::string place = tacGenerator.emitBinary(">", $1->place, $3->place);
                if (!areTypesCompatible($1->typeName, $3->typeName)) {
                    printf("Semantic Error at line %d: incompatible types for comparison (%s, %s)\n", yylineno, $1->typeName.c_str(), $3->typeName.c_str());
                }
                $$ = makeExpr(place, ($1->value > $3->value) ? 1.0 : 0.0, $1->isConst && $3->isConst, false, "FLAG");
                delete $1;
                delete $3;
            }
        | expression LE expression
            {
                std::string place = tacGenerator.emitBinary("<=", $1->place, $3->place);
                if (!areTypesCompatible($1->typeName, $3->typeName)) {
                    printf("Semantic Error at line %d: incompatible types for comparison (%s, %s)\n", yylineno, $1->typeName.c_str(), $3->typeName.c_str());
                }
                $$ = makeExpr(place, ($1->value <= $3->value) ? 1.0 : 0.0, $1->isConst && $3->isConst, false, "FLAG");
                delete $1;
                delete $3;
            }
        | expression GE expression
            {
                std::string place = tacGenerator.emitBinary(">=", $1->place, $3->place);
                if (!areTypesCompatible($1->typeName, $3->typeName)) {
                    printf("Semantic Error at line %d: incompatible types for comparison (%s, %s)\n", yylineno, $1->typeName.c_str(), $3->typeName.c_str());
                }
                $$ = makeExpr(place, ($1->value >= $3->value) ? 1.0 : 0.0, $1->isConst && $3->isConst, false, "FLAG");
                delete $1;
                delete $3;
            }
        | expression EQ expression
            {
                std::string place = tacGenerator.emitBinary("==", $1->place, $3->place);
                if (!areTypesCompatible($1->typeName, $3->typeName)) {
                    printf("Semantic Error at line %d: incompatible types for equality (%s, %s)\n", yylineno, $1->typeName.c_str(), $3->typeName.c_str());
                }
                $$ = makeExpr(place, ($1->value == $3->value) ? 1.0 : 0.0, $1->isConst && $3->isConst, false, "FLAG");
                delete $1;
                delete $3;
            }
        | expression NEQ expression
            {
                std::string place = tacGenerator.emitBinary("!=", $1->place, $3->place);
                if (!areTypesCompatible($1->typeName, $3->typeName)) {
                    printf("Semantic Error at line %d: incompatible types for inequality (%s, %s)\n", yylineno, $1->typeName.c_str(), $3->typeName.c_str());
                }
                $$ = makeExpr(place, ($1->value != $3->value) ? 1.0 : 0.0, $1->isConst && $3->isConst, false, "FLAG");
                delete $1;
                delete $3;
            }
        | expression ADD expression
      {
          if (!isNumericType($1->typeName) && !isStringType($1->typeName)) {
              printf("Semantic Error at line %d: invalid left operand type for add: %s\n", yylineno, $1->typeName.c_str());
          }
          if (!isNumericType($3->typeName) && !isStringType($3->typeName)) {
              printf("Semantic Error at line %d: invalid right operand type for add: %s\n", yylineno, $3->typeName.c_str());
          }
          std::string place = tacGenerator.emitBinary("+", $1->place, $3->place);
          std::string resultType = (isStringType($1->typeName) || isStringType($3->typeName))
              ? "SYMBOL"
              : numericResultType($1->typeName, $3->typeName);
          $$ = makeExpr(place, 0.0, false, false, resultType);
          delete $1;
          delete $3;
      }
    | expression MINUS expression
      {
          if (!isNumericType($1->typeName) || !isNumericType($3->typeName)) {
              printf("Semantic Error at line %d: minus requires numeric operands\n", yylineno);
          }
          std::string place = tacGenerator.emitBinary("-", $1->place, $3->place);
          $$ = makeExpr(place, 0.0, false, false, numericResultType($1->typeName, $3->typeName));
          delete $1;
          delete $3;
      }
    | expression MUL expression
      {
          if (!isNumericType($1->typeName) || !isNumericType($3->typeName)) {
              printf("Semantic Error at line %d: mul requires numeric operands\n", yylineno);
          }
          std::string place = tacGenerator.emitBinary("*", $1->place, $3->place);
          $$ = makeExpr(place, 0.0, false, false, numericResultType($1->typeName, $3->typeName));
          delete $1;
          delete $3;
      }
    | expression DIVIDE expression
      {
          if (!isNumericType($1->typeName) || !isNumericType($3->typeName)) {
              printf("Semantic Error at line %d: divide requires numeric operands\n", yylineno);
          }
          std::string place = tacGenerator.emitBinary("/", $1->place, $3->place);
          $$ = makeExpr(place, 0.0, false, false, numericResultType($1->typeName, $3->typeName));
          delete $1;
          delete $3;
      }
        | expression MOD expression
            {
                    if (!isNumericType($1->typeName) || !isNumericType($3->typeName)) {
                            printf("Semantic Error at line %d: mod requires numeric operands\n", yylineno);
                    }
                    std::string place = tacGenerator.emitBinary("%", $1->place, $3->place);
                                        $$ = makeExpr(place, 0.0, false, false, "COUNT");
                    delete $1;
                    delete $3;
            }
        | expression POWER expression
            {
                    if (!isNumericType($1->typeName) || !isNumericType($3->typeName)) {
                            printf("Semantic Error at line %d: power requires numeric operands\n", yylineno);
                    }
                    std::string place = tacGenerator.emitBinary("**", $1->place, $3->place);
                                        $$ = makeExpr(place, 0.0, false, false, numericResultType($1->typeName, $3->typeName));
                    delete $1;
                    delete $3;
            }
        | expression AND expression
            {
                    if (!isNumericType($1->typeName) || !isNumericType($3->typeName)) {
                            printf("Semantic Error at line %d: AND requires boolean/numeric operands\n", yylineno);
                    }
                    std::string place = tacGenerator.emitBinary("AND", $1->place, $3->place);
                                        $$ = makeExpr(place, 0.0, false, false, "FLAG");
                    delete $1;
                    delete $3;
            }
        | expression OR expression
            {
                    if (!isNumericType($1->typeName) || !isNumericType($3->typeName)) {
                            printf("Semantic Error at line %d: OR requires boolean/numeric operands\n", yylineno);
                    }
                    std::string place = tacGenerator.emitBinary("OR", $1->place, $3->place);
                                        $$ = makeExpr(place, 0.0, false, false, "FLAG");
                    delete $1;
                    delete $3;
            }
        | expression XOR expression
            {
                    if (!isNumericType($1->typeName) || !isNumericType($3->typeName)) {
                            printf("Semantic Error at line %d: XOR requires boolean/numeric operands\n", yylineno);
                    }
                    std::string place = tacGenerator.emitBinary("XOR", $1->place, $3->place);
                                        $$ = makeExpr(place, 0.0, false, false, "FLAG");
                    delete $1;
                    delete $3;
            }
        | NOT expression
            {
                    if (!isNumericType($2->typeName) && !isBoolType($2->typeName)) {
                            printf("Semantic Error at line %d: NOT requires boolean/numeric operand\n", yylineno);
                    }
                    std::string place = tacGenerator.emitBinary("==", $2->place, "0");
                                        $$ = makeExpr(place, 0.0, false, false, "FLAG");
                    delete $2;
            }
    | ROOT LPAREN expression RPAREN
      {
          if (!isNumericType($3->typeName)) {
              printf("Semantic Error at line %d: root() requires numeric operand\n", yylineno);
          }
          std::string place = tacGenerator.emitUnary("root", $3->place);
          $$ = makeExpr(place, 0.0, false, false, "REAL");
          delete $3;
      }
    | FLR LPAREN expression RPAREN
      {
          if (!isNumericType($3->typeName)) {
              printf("Semantic Error at line %d: flr() requires numeric operand\n", yylineno);
          }
          std::string place = tacGenerator.emitUnary("flr", $3->place);
          $$ = makeExpr(place, 0.0, false, false, "COUNT");
          delete $3;
      }
    | CEIL LPAREN expression RPAREN
      {
          if (!isNumericType($3->typeName)) {
              printf("Semantic Error at line %d: ceil() requires numeric operand\n", yylineno);
          }
          std::string place = tacGenerator.emitUnary("ceil", $3->place);
          $$ = makeExpr(place, 0.0, false, false, "COUNT");
          delete $3;
      }
    | ABS LPAREN expression RPAREN
      {
          if (!isNumericType($3->typeName)) {
              printf("Semantic Error at line %d: abs() requires numeric operand\n", yylineno);
          }
          std::string place = tacGenerator.emitUnary("abs", $3->place);
          $$ = makeExpr(place, 0.0, false, false, numericResultType($3->typeName, $3->typeName));
          delete $3;
      }
    | LOGARITHM LPAREN expression RPAREN
      {
          if (!isNumericType($3->typeName)) {
              printf("Semantic Error at line %d: logarithm() requires numeric operand\n", yylineno);
          }
          std::string place = tacGenerator.emitUnary("logarithm", $3->place);
          $$ = makeExpr(place, 0.0, false, false, "REAL");
          delete $3;
      }
    | SINE LPAREN expression RPAREN
      {
          if (!isNumericType($3->typeName)) {
              printf("Semantic Error at line %d: sine() requires numeric operand\n", yylineno);
          }
          std::string place = tacGenerator.emitUnary("sine", $3->place);
          $$ = makeExpr(place, 0.0, false, false, "REAL");
          delete $3;
      }
    | COSINE LPAREN expression RPAREN
      {
          if (!isNumericType($3->typeName)) {
              printf("Semantic Error at line %d: cosine() requires numeric operand\n", yylineno);
          }
          std::string place = tacGenerator.emitUnary("cosine", $3->place);
          $$ = makeExpr(place, 0.0, false, false, "REAL");
          delete $3;
      }
    | TAN LPAREN expression RPAREN
      {
          if (!isNumericType($3->typeName)) {
              printf("Semantic Error at line %d: tan() requires numeric operand\n", yylineno);
          }
          std::string place = tacGenerator.emitUnary("tan", $3->place);
          $$ = makeExpr(place, 0.0, false, false, "REAL");
          delete $3;
      }
    | ASINE LPAREN expression RPAREN
      {
          if (!isNumericType($3->typeName)) {
              printf("Semantic Error at line %d: asine() requires numeric operand\n", yylineno);
          }
          std::string place = tacGenerator.emitUnary("asine", $3->place);
          $$ = makeExpr(place, 0.0, false, false, "REAL");
          delete $3;
      }
    | ACOSINE LPAREN expression RPAREN
      {
          if (!isNumericType($3->typeName)) {
              printf("Semantic Error at line %d: acosine() requires numeric operand\n", yylineno);
          }
          std::string place = tacGenerator.emitUnary("acosine", $3->place);
          $$ = makeExpr(place, 0.0, false, false, "REAL");
          delete $3;
      }
    | ATAN LPAREN expression RPAREN
      {
          if (!isNumericType($3->typeName)) {
              printf("Semantic Error at line %d: atan() requires numeric operand\n", yylineno);
          }
          std::string place = tacGenerator.emitUnary("atan", $3->place);
          $$ = makeExpr(place, 0.0, false, false, "REAL");
          delete $3;
      }
    | PRIME LPAREN expression RPAREN
      {
          if (!isNumericType($3->typeName)) {
              printf("Semantic Error at line %d: prime() requires numeric operand\n", yylineno);
          }
          std::string place = tacGenerator.emitUnary("prime", $3->place);
          $$ = makeExpr(place, 0.0, false, false, "FLAG");
          delete $3;
      }
    | INT_LITERAL
      {
          $$ = makeExpr(toText(static_cast<double>($1)), static_cast<double>($1), true, false, "COUNT");
      }
    | FLOAT_LITERAL
      {
          $$ = makeExpr(toText($1), $1, true, false, "REAL");
      }
        | STRING_LITERAL
            {
                    $$ = makeExpr($1, 0.0, true, false, "SYMBOL");
            }
    | THIS
      {
          if (!currentFunctionInModule) {
              printf("Semantic Error at line %d: this used outside module function context\n", yylineno);
          }
          $$ = makeExpr("\"this\"", 0.0, true, false, "SYMBOL");
      }
    | IDENTIFIER
      {
          if (!isDeclaredName($1)) {
              printf("Semantic Error at line %d: variable %s not declared\n", yylineno, $1);
              $$ = makeExpr($1, 0.0, false, false, "UNKNOWN");
          } else {
                            $$ = makeExpr($1, 0.0, false, true, declaredTypeOf($1));
          }
      }
        | IDENTIFIER LBRACKET expression RBRACKET
            {
                      if (!isDeclaredName($1)) {
                            printf("Semantic Error at line %d: array %s not declared\n", yylineno, $1);
                            $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
                    } else {
                            std::string temp = tacGenerator.emitArrayLoad($1, $3->place);
                            $$ = makeExpr(temp, 0.0, false, false, baseType(declaredTypeOf($1)));
                    }
                    delete $3;
            }
        | IDENTIFIER LPAREN argument_list_opt RPAREN
            {
                auto arityIt = functionArity.find($1);
                if (arityIt != functionArity.end() && arityIt->second != $3) {
                    printf("Semantic Error at line %d: function %s expects %d arguments but got %d\n", yylineno, $1, arityIt->second, $3);
                }
                    std::string callTemp = tacGenerator.emitCall($1, $3);
                    std::string returnType = "UNKNOWN";
                    auto returnTypeIt = functionReturnTypes.find($1);
                    if (returnTypeIt != functionReturnTypes.end()) {
                        returnType = returnTypeIt->second;
                    }
                    $$ = makeExpr(callTemp, 0.0, false, false, returnType);
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
