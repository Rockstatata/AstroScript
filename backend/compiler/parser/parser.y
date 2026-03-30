%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
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
extern char* yytext;

SymbolTable symbolTable;
TACGenerator tacGenerator;

struct ExprAttr {
    std::string place;
    double value;
    bool isConst;
    bool isIdentifier;
    std::string typeName;
};

struct FieldInfo {
    std::string typeName;
    bool isPublic;
    bool isConstant;
    bool hasDefault;
    std::string defaultPlace;
};

struct MethodInfo {
    std::string ownerModule;
    std::string qualifiedName;
    std::vector<std::string> parameterTypes;
    std::string returnType;
    bool isPublic;
    bool isOverride;
    bool isConstructor;
    int arity;
    std::string selector;
};

struct FunctionOverloadInfo {
    std::string qualifiedName;
    std::vector<std::string> parameterTypes;
    std::string returnType;
    int arity;
};

struct ModuleInfo {
    std::string name;
    std::string baseName;
    std::unordered_map<std::string, FieldInfo> fields;
    std::unordered_map<std::string, std::vector<MethodInfo>> methods;
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
std::vector<std::string> currentFunctionParamTypesInOrder;
std::string currentFunctionName;
std::string currentFunctionQualifiedName;
std::string currentFunctionReturnType = "VOIDSPACE";
bool currentFunctionInModule = false;
bool currentFunctionIsConstructor = false;
bool currentFunctionIsMethod = false;
std::string currentFunctionOwnerModule;
std::unordered_map<std::string, int> functionArity;
std::unordered_map<std::string, std::unordered_map<std::string, std::string>> functionParameterTypes;
std::unordered_map<std::string, std::string> functionReturnTypes;
std::unordered_map<std::string, std::vector<FunctionOverloadInfo>> globalFunctionOverloads;
std::unordered_map<std::string, std::string> typeAliases;
std::unordered_set<std::string> constantNames;
std::unordered_map<std::string, ModuleInfo> moduleTable;
std::vector<std::unordered_map<std::string, std::string>> variableScopes;
std::vector<std::unordered_map<std::string, int>> variableScopeLines;
std::vector<std::unordered_set<std::string>> constantScopes;
std::string currentModuleName;
bool currentClassMemberIsPublic = true;
bool currentClassMemberIsOverride = false;
std::string pendingModuleName;
int moduleDepth = 0;
bool currentFunctionSawValueReturn = false;

std::string qualifyMethodName(const std::string& moduleName, const std::string& methodName) {
    return moduleName + "::" + methodName;
}

std::string buildSelector(const std::string& methodName, int arity) {
    return methodName + "#" + std::to_string(arity);
}

std::string buildQualifiedFunctionName(const std::string& owner, const std::string& functionName, int arity) {
    if (!owner.empty()) {
        return owner + "::" + functionName + "@" + std::to_string(arity);
    }
    return functionName + "@" + std::to_string(arity);
}

void pushScope() {
    variableScopes.push_back({});
    variableScopeLines.push_back({});
    constantScopes.push_back({});
}

void popScope() {
    if (!variableScopes.empty()) {
        variableScopes.pop_back();
    }
    if (!variableScopeLines.empty()) {
        variableScopeLines.pop_back();
    }
    if (!constantScopes.empty()) {
        constantScopes.pop_back();
    }
}

bool isDeclaredInCurrentScopeOnly(const std::string& name) {
    if (variableScopes.empty()) {
        return false;
    }
    return variableScopes.back().find(name) != variableScopes.back().end();
}

int declaredLineInCurrentScope(const std::string& name) {
    if (variableScopeLines.empty()) {
        return -1;
    }

    auto lineIt = variableScopeLines.back().find(name);
    if (lineIt == variableScopeLines.back().end()) {
        return -1;
    }

    return lineIt->second;
}

void reportRedeclaration(const std::string& name) {
    int previousLine = declaredLineInCurrentScope(name);
    if (previousLine > 0) {
        printf("Semantic Error at line %d: variable %s already declared in this scope at line %d\n", yylineno, name.c_str(), previousLine);
    } else {
        printf("Semantic Error at line %d: variable %s already declared\n", yylineno, name.c_str());
    }
}

bool declareScopedName(const std::string& name, const std::string& typeName, bool isConstant, int lineNumber) {
    if (isDeclaredInCurrentScopeOnly(name)) {
        return false;
    }

    if (variableScopes.empty()) {
        pushScope();
    }

    variableScopes.back()[name] = typeName;
    variableScopeLines.back()[name] = lineNumber;
    if (isConstant) {
        constantScopes.back().insert(name);
        constantNames.insert(name);
    }

    if (!symbolTable.exists(name)) {
        symbolTable.insert(name, typeName, lineNumber);
    }

    return true;
}

bool isConstantName(const std::string& name) {
    for (auto scopeIt = constantScopes.rbegin(); scopeIt != constantScopes.rend(); ++scopeIt) {
        if (scopeIt->find(name) != scopeIt->end()) {
            return true;
        }
    }
    return constantNames.find(name) != constantNames.end();
}

bool isModuleKnown(const std::string& moduleName) {
    return moduleTable.find(moduleName) != moduleTable.end();
}

bool isModuleType(const std::string& typeName) {
    return typeName.rfind("MODULE:", 0) == 0;
}

std::string moduleNameFromType(const std::string& typeName) {
    if (!isModuleType(typeName)) {
        return "";
    }
    return typeName.substr(std::string("MODULE:").size());
}

bool isDerivedFrom(const std::string& childModule, const std::string& parentModule) {
    if (childModule.empty() || parentModule.empty()) {
        return false;
    }

    std::string cursor = childModule;
    while (!cursor.empty()) {
        if (cursor == parentModule) {
            return true;
        }
        auto moduleIt = moduleTable.find(cursor);
        if (moduleIt == moduleTable.end()) {
            break;
        }
        cursor = moduleIt->second.baseName;
    }

    return false;
}

bool resolveFieldInHierarchy(const std::string& moduleName,
                            const std::string& fieldName,
                            FieldInfo& resolvedField,
                            std::string& ownerModule) {
    std::string cursor = moduleName;
    while (!cursor.empty()) {
        auto moduleIt = moduleTable.find(cursor);
        if (moduleIt == moduleTable.end()) {
            break;
        }
        auto fieldIt = moduleIt->second.fields.find(fieldName);
        if (fieldIt != moduleIt->second.fields.end()) {
            resolvedField = fieldIt->second;
            ownerModule = cursor;
            return true;
        }
        cursor = moduleIt->second.baseName;
    }
    return false;
}

bool resolveMethodInHierarchy(const std::string& moduleName,
                             const std::string& methodName,
                             int argumentCount,
                             MethodInfo& resolvedMethod,
                             std::string& ownerModule) {
    std::string cursor = moduleName;
    while (!cursor.empty()) {
        auto moduleIt = moduleTable.find(cursor);
        if (moduleIt == moduleTable.end()) {
            break;
        }
        auto methodGroupIt = moduleIt->second.methods.find(methodName);
        if (methodGroupIt != moduleIt->second.methods.end()) {
            for (const MethodInfo& candidate : methodGroupIt->second) {
                if (candidate.arity == argumentCount) {
                    resolvedMethod = candidate;
                    ownerModule = cursor;
                    return true;
                }
            }
        }
        cursor = moduleIt->second.baseName;
    }
    return false;
}

bool resolveGlobalFunctionOverload(const std::string& functionName,
                                  int argumentCount,
                                  FunctionOverloadInfo& resolved) {
    auto functionIt = globalFunctionOverloads.find(functionName);
    if (functionIt == globalFunctionOverloads.end()) {
        return false;
    }

    for (const FunctionOverloadInfo& overload : functionIt->second) {
        if (overload.arity == argumentCount) {
            resolved = overload;
            return true;
        }
    }

    return false;
}

bool canAccessMember(bool isPublicMember, const std::string& ownerModule) {
    if (isPublicMember) {
        return true;
    }
    return currentFunctionInModule && currentFunctionOwnerModule == ownerModule;
}

bool isParameterName(const std::string& name) {
    for (const std::string& param : currentFunctionParams) {
        if (param == name) {
            return true;
        }
    }
    return false;
}

bool isDeclaredName(const std::string& name) {
    if (isParameterName(name)) {
        return true;
    }

    for (auto scopeIt = variableScopes.rbegin(); scopeIt != variableScopes.rend(); ++scopeIt) {
        if (scopeIt->find(name) != scopeIt->end()) {
            return true;
        }
    }

    return false;
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

    for (auto scopeIt = variableScopes.rbegin(); scopeIt != variableScopes.rend(); ++scopeIt) {
        auto valueIt = scopeIt->find(name);
        if (valueIt != scopeIt->end()) {
            return valueIt->second;
        }
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

    if (isModuleType(target) && isModuleType(source)) {
        const std::string targetModule = moduleNameFromType(target);
        const std::string sourceModule = moduleNameFromType(source);
        return isDerivedFrom(sourceModule, targetModule);
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
%token MODULE DEPLOY EXTENDS PUBLIC PRIVATE THIS NEW SUPER OVERRIDE
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
%token MEMBER_DOT DOT COMMA COLON

%token <intval> INT_LITERAL
%token <floatval> FLOAT_LITERAL
%token <str> IDENTIFIER STRING_LITERAL

%type <str> type
%type <expr> expression
%type <expr> field_initializer
%type <intval> argument_list argument_list_opt
%type <intval> parameter_list parameter_list_opt

%error-verbose

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
    : LBRACE
      {
          pushScope();
          tacGenerator.emitScopeBegin();
      }
      statement_list RBRACE
      {
          tacGenerator.emitScopeEnd();
          popScope();
      }
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
    | error DOT
      {
          printf("SYNTAX ERROR at line %d: invalid statement; skipped until '.' to continue parsing\n", yylineno);
          yyerrok;
      }
    | return_statement
    | ABORT DOT
    ;

declaration
    : TELEMETRY type IDENTIFIER DOT
      {
          if (!declareScopedName($3, $2, false, yylineno)) {
              reportRedeclaration($3);
          } else {
              tacGenerator.emitDeclare($2, $3);
          }
      }
    | TELEMETRY type IDENTIFIER ASSIGN expression DOT
      {
          if (!declareScopedName($3, $2, false, yylineno)) {
              reportRedeclaration($3);
          } else {
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
          if (!declareScopedName($3, $2, true, yylineno)) {
              reportRedeclaration($3);
          } else {
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
          const std::string arrayType = std::string($2) + "[]";
          if (!declareScopedName($3, arrayType, false, yylineno)) {
              reportRedeclaration($3);
          } else {
              // Unsized fleet defaults to a small usable capacity for indexed operations.
              tacGenerator.emitArrayDecl($2, $3, "8");
          }
      }
    | FLEET type IDENTIFIER LBRACKET INT_LITERAL RBRACKET DOT
      {
          const std::string arrayType = std::string($2) + "[]";
          if (!declareScopedName($3, arrayType, false, yylineno)) {
              reportRedeclaration($3);
          } else {
              tacGenerator.emitArrayDecl($2, $3, toText(static_cast<double>($5)));
          }
      }
    | FLEET type IDENTIFIER LBRACKET INT_LITERAL RBRACKET ASSIGN expression DOT
      {
          const std::string arrayType = std::string($2) + "[]";
          if (!declareScopedName($3, arrayType, false, yylineno)) {
              reportRedeclaration($3);
          } else {
              tacGenerator.emitArrayDecl($2, $3, toText(static_cast<double>($5)));
              tacGenerator.emitArrayStore($3, "0", $8->place);
          }
          delete $8;
      }
    | MODE IDENTIFIER LBRACE mode_body RBRACE
    | TELEMETRY type IDENTIFIER LBRACKET INT_LITERAL RBRACKET DOT
      {
          const std::string arrayType = std::string($2) + "[]";
          if (!declareScopedName($3, arrayType, false, yylineno)) {
              reportRedeclaration($3);
          } else {
              tacGenerator.emitArrayDecl($2, $3, toText(static_cast<double>($5)));
          }
      }
    | TELEMETRY type IDENTIFIER LBRACKET INT_LITERAL RBRACKET ASSIGN expression DOT
      {
          const std::string arrayType = std::string($2) + "[]";
          if (!declareScopedName($3, arrayType, false, yylineno)) {
              reportRedeclaration($3);
          } else {
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
              if (currentFunctionInModule) {
                  FieldInfo fieldInfo;
                  std::string ownerModule;
                  if (resolveFieldInHierarchy(currentFunctionOwnerModule, $1, fieldInfo, ownerModule)) {
                      if (!canAccessMember(fieldInfo.isPublic, ownerModule)) {
                          printf("Semantic Error at line %d: field %s is private in module %s\n", yylineno, $1, ownerModule.c_str());
                      } else if (fieldInfo.isConstant) {
                          printf("Semantic Error at line %d: cannot assign to constant field %s\n", yylineno, $1);
                      } else {
                          if (!areTypesCompatible(fieldInfo.typeName, $3->typeName)) {
                              printf("Semantic Error at line %d: type mismatch assigning %s to field %s (%s)\n", yylineno, $3->typeName.c_str(), $1, fieldInfo.typeName.c_str());
                          }
                          tacGenerator.emitFieldStore("this", $1, $3->place);
                      }
                  } else {
                      printf("Semantic Error at line %d: variable %s not declared\n", yylineno, $1);
                  }
              } else {
                  printf("Semantic Error at line %d: variable %s not declared\n", yylineno, $1);
              }
          } else if (isConstantName($1)) {
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
    | IDENTIFIER MEMBER_DOT IDENTIFIER ASSIGN expression DOT
      {
          if (!isDeclaredName($1)) {
              printf("Semantic Error at line %d: object %s not declared\n", yylineno, $1);
          } else {
              std::string objectType = declaredTypeOf($1);
              if (!isModuleType(objectType)) {
                  printf("Semantic Error at line %d: %s is not an object instance\n", yylineno, $1);
              } else {
                  const std::string objectModule = moduleNameFromType(objectType);
                  FieldInfo fieldInfo;
                  std::string ownerModule;
                  if (!resolveFieldInHierarchy(objectModule, $3, fieldInfo, ownerModule)) {
                      printf("Semantic Error at line %d: field %s not found in module %s\n", yylineno, $3, objectModule.c_str());
                  } else if (!canAccessMember(fieldInfo.isPublic, ownerModule)) {
                      printf("Semantic Error at line %d: field %s is private in module %s\n", yylineno, $3, ownerModule.c_str());
                  } else if (fieldInfo.isConstant) {
                      printf("Semantic Error at line %d: cannot assign to constant field %s\n", yylineno, $3);
                  } else {
                      if (!areTypesCompatible(fieldInfo.typeName, $5->typeName)) {
                          printf("Semantic Error at line %d: type mismatch assigning %s to field %s (%s)\n", yylineno, $5->typeName.c_str(), $3, fieldInfo.typeName.c_str());
                      }
                      tacGenerator.emitFieldStore($1, $3, $5->place);
                  }
              }
          }
          delete $5;
      }
    | THIS MEMBER_DOT IDENTIFIER ASSIGN expression DOT
      {
          if (!currentFunctionInModule) {
              printf("Semantic Error at line %d: this used outside module function context\n", yylineno);
          } else {
              FieldInfo fieldInfo;
              std::string ownerModule;
              if (!resolveFieldInHierarchy(currentFunctionOwnerModule, $3, fieldInfo, ownerModule)) {
                  printf("Semantic Error at line %d: field %s not found in module %s\n", yylineno, $3, currentFunctionOwnerModule.c_str());
              } else if (!canAccessMember(fieldInfo.isPublic, ownerModule)) {
                  printf("Semantic Error at line %d: field %s is private in module %s\n", yylineno, $3, ownerModule.c_str());
              } else if (fieldInfo.isConstant) {
                  printf("Semantic Error at line %d: cannot assign to constant field %s\n", yylineno, $3);
              } else {
                  if (!areTypesCompatible(fieldInfo.typeName, $5->typeName)) {
                      printf("Semantic Error at line %d: type mismatch assigning %s to field %s (%s)\n", yylineno, $5->typeName.c_str(), $3, fieldInfo.typeName.c_str());
                  }
                  tacGenerator.emitFieldStore("this", $3, $5->place);
              }
          }
          delete $5;
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
          const std::string deployedType = std::string("MODULE:") + $2;
          if (!isModuleKnown($2)) {
              printf("Semantic Error at line %d: module %s is not defined\n", yylineno, $2);
          } else if (!declareScopedName($3, deployedType, false, yylineno)) {
              reportRedeclaration($3);
          } else {
              tacGenerator.emitDeclare(deployedType, $3);
              tacGenerator.emitObjectNew($2, $3);

              MethodInfo ctorInfo;
              std::string ctorOwner;
              if (resolveMethodInHierarchy($2, $2, 0, ctorInfo, ctorOwner)) {
                  std::string initTemp = tacGenerator.emitMethodCall($3, ctorInfo.selector, 0);
                  (void)initTemp;
              }
          }
      }
    | DEPLOY IDENTIFIER IDENTIFIER LPAREN argument_list_opt RPAREN DOT
      {
          const std::string deployedType = std::string("MODULE:") + $2;
          if (!isModuleKnown($2)) {
              printf("Semantic Error at line %d: module %s is not defined\n", yylineno, $2);
          } else if (!declareScopedName($3, deployedType, false, yylineno)) {
              reportRedeclaration($3);
          } else {
              tacGenerator.emitDeclare(deployedType, $3);
              tacGenerator.emitObjectNew($2, $3);

              MethodInfo ctorInfo;
              std::string ctorOwner;
              if (resolveMethodInHierarchy($2, $2, $5, ctorInfo, ctorOwner)) {
                  if (ctorInfo.arity != $5) {
                      printf("Semantic Error at line %d: constructor %s expects %d arguments but got %d\n", yylineno, $2, static_cast<int>(ctorInfo.parameterTypes.size()), $5);
                  }
                  std::string initTemp = tacGenerator.emitMethodCall($3, ctorInfo.selector, $5);
                  (void)initTemp;
              } else if ($5 != 0) {
                  printf("Semantic Error at line %d: module %s has no constructor accepting %d arguments\n", yylineno, $2, $5);
              }
          }
      }
    | DEPLOY IDENTIFIER DOT
      {
          const std::string deployedType = std::string("MODULE:") + $2;
          if (!isModuleKnown($2)) {
              printf("Semantic Error at line %d: module %s is not defined\n", yylineno, $2);
          } else if (!declareScopedName($2, deployedType, false, yylineno)) {
              reportRedeclaration($2);
          } else {
              tacGenerator.emitDeclare(deployedType, $2);
              tacGenerator.emitObjectNew($2, $2);

              MethodInfo ctorInfo;
              std::string ctorOwner;
              if (resolveMethodInHierarchy($2, $2, 0, ctorInfo, ctorOwner)) {
                  std::string initTemp = tacGenerator.emitMethodCall($2, ctorInfo.selector, 0);
                  (void)initTemp;
              }
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
          } else if (isConstantName($2)) {
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
                    if (aliasIt != typeAliases.end()) {
              $$ = strdup(aliasIt->second.c_str());
                    } else if (isModuleKnown($1)) {
                            std::string moduleType = std::string("MODULE:") + $1;
                            $$ = strdup(moduleType.c_str());
                    } else {
                            printf("Semantic Error at line %d: unknown type alias/module %s\n", yylineno, $1);
                            $$ = strdup("COUNT");
          }
      }
    ;

field_initializer
        : INT_LITERAL
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
          pendingModuleName = $2;

          if (isModuleKnown($2)) {
              printf("Semantic Error at line %d: module %s already defined\n", yylineno, $2);
          } else {
              ModuleInfo moduleInfo;
              moduleInfo.name = $2;
              moduleTable[$2] = moduleInfo;
          }

          ++moduleDepth;
          currentModuleName = $2;
          currentClassMemberIsPublic = true;
          currentClassMemberIsOverride = false;
      }
      inheritance LBRACE class_body RBRACE
      {
          const auto moduleIt = moduleTable.find(currentModuleName);
          if (moduleIt != moduleTable.end()) {
              tacGenerator.registerClass(moduleIt->second.name, moduleIt->second.baseName);
          }

          --moduleDepth;
          currentModuleName.clear();
          pendingModuleName.clear();
          currentClassMemberIsPublic = true;
          currentClassMemberIsOverride = false;
      }
    ;

inheritance
    : EXTENDS IDENTIFIER
      {
          auto currentModuleIt = moduleTable.find(pendingModuleName);
          if (currentModuleIt == moduleTable.end()) {
              printf("Semantic Error at line %d: invalid module inheritance context for %s\n", yylineno, pendingModuleName.c_str());
          } else if (!isModuleKnown($2)) {
              printf("Semantic Error at line %d: base module %s is not defined\n", yylineno, $2);
          } else if (std::string($2) == pendingModuleName || isDerivedFrom($2, pendingModuleName)) {
              printf("Semantic Error at line %d: cyclic inheritance involving %s and %s\n", yylineno, pendingModuleName.c_str(), $2);
          } else {
              currentModuleIt->second.baseName = $2;
          }
      }
    |
      {
      }
    ;

class_body
    : class_body class_member
    |
    ;

class_member
    :
      {
          currentClassMemberIsPublic = true;
          currentClassMemberIsOverride = false;
      }
      class_member_core
    | PUBLIC
      {
          currentClassMemberIsPublic = true;
          currentClassMemberIsOverride = false;
      }
      class_member_core
    | PRIVATE
      {
          currentClassMemberIsPublic = false;
          currentClassMemberIsOverride = false;
      }
      class_member_core
    | OVERRIDE
      {
          currentClassMemberIsPublic = true;
          currentClassMemberIsOverride = true;
      }
      function_definition
    | PUBLIC OVERRIDE
      {
          currentClassMemberIsPublic = true;
          currentClassMemberIsOverride = true;
      }
      function_definition
    | PRIVATE OVERRIDE
      {
          currentClassMemberIsPublic = false;
          currentClassMemberIsOverride = true;
      }
      function_definition
    ;

class_member_core
    : TELEMETRY type IDENTIFIER DOT
      {
          auto moduleIt = moduleTable.find(currentModuleName);
          if (moduleIt == moduleTable.end()) {
              printf("Semantic Error at line %d: invalid field declaration context for %s\n", yylineno, $3);
          } else if (moduleIt->second.fields.find($3) != moduleIt->second.fields.end()) {
              printf("Semantic Error at line %d: field %s already defined in module %s\n", yylineno, $3, currentModuleName.c_str());
          } else {
              FieldInfo fieldInfo{$2, currentClassMemberIsPublic, false, false, ""};
              moduleIt->second.fields[$3] = fieldInfo;
              tacGenerator.registerField(currentModuleName, $3, "0");
          }
      }
    | TELEMETRY type IDENTIFIER ASSIGN field_initializer DOT
      {
          auto moduleIt = moduleTable.find(currentModuleName);
          if (moduleIt == moduleTable.end()) {
              printf("Semantic Error at line %d: invalid field declaration context for %s\n", yylineno, $3);
          } else if (moduleIt->second.fields.find($3) != moduleIt->second.fields.end()) {
              printf("Semantic Error at line %d: field %s already defined in module %s\n", yylineno, $3, currentModuleName.c_str());
          } else {
              if (!areTypesCompatible($2, $5->typeName)) {
                  printf("Semantic Error at line %d: cannot initialize field %s (%s) with %s\n", yylineno, $3, $2, $5->typeName.c_str());
              }
              FieldInfo fieldInfo{$2, currentClassMemberIsPublic, false, true, $5->place};
              moduleIt->second.fields[$3] = fieldInfo;
              tacGenerator.registerField(currentModuleName, $3, $5->place);
          }
          delete $5;
      }
    | LIMIT type IDENTIFIER ASSIGN field_initializer DOT
      {
          auto moduleIt = moduleTable.find(currentModuleName);
          if (moduleIt == moduleTable.end()) {
              printf("Semantic Error at line %d: invalid field declaration context for %s\n", yylineno, $3);
          } else if (moduleIt->second.fields.find($3) != moduleIt->second.fields.end()) {
              printf("Semantic Error at line %d: field %s already defined in module %s\n", yylineno, $3, currentModuleName.c_str());
          } else {
              if (!areTypesCompatible($2, $5->typeName)) {
                  printf("Semantic Error at line %d: cannot initialize constant field %s (%s) with %s\n", yylineno, $3, $2, $5->typeName.c_str());
              }
              FieldInfo fieldInfo{$2, currentClassMemberIsPublic, true, true, $5->place};
              moduleIt->second.fields[$3] = fieldInfo;
              tacGenerator.registerField(currentModuleName, $3, $5->place);
          }
          delete $5;
      }
    | function_definition
    ;

function_definition
    : COMMAND IDENTIFIER LPAREN
      {
          currentFunctionName = $2;
          currentFunctionParams.clear();
          currentFunctionParamTypes.clear();
          currentFunctionParamTypesInOrder.clear();
          currentFunctionInModule = moduleDepth > 0;
          currentFunctionSawValueReturn = false;
          currentFunctionOwnerModule = currentFunctionInModule ? currentModuleName : "";
          currentFunctionIsMethod = currentFunctionInModule;
          currentFunctionIsConstructor = currentFunctionInModule && (currentFunctionName == currentModuleName);
          currentFunctionQualifiedName.clear();
      }
      parameter_list_opt RPAREN return_type type
      {
          if (currentFunctionIsConstructor && std::string($8) != "VOIDSPACE") {
              printf("Semantic Error at line %d: constructor %s must have return type voidspace\n", yylineno, currentFunctionName.c_str());
          }

          currentFunctionQualifiedName = buildQualifiedFunctionName(
              currentFunctionInModule ? currentFunctionOwnerModule : "",
              currentFunctionName,
              $5);

          functionArity[currentFunctionQualifiedName] = currentFunctionIsMethod ? ($5 + 1) : $5;
          functionParameterTypes[currentFunctionQualifiedName] = currentFunctionParamTypes;
          functionReturnTypes[currentFunctionQualifiedName] = $8;
          currentFunctionReturnType = $8;

          if (currentFunctionInModule) {
              currentFunctionParamTypes["this"] = std::string("MODULE:") + currentFunctionOwnerModule;

              auto moduleIt = moduleTable.find(currentFunctionOwnerModule);
              if (moduleIt == moduleTable.end()) {
                  printf("Semantic Error at line %d: invalid module function context for %s\n", yylineno, currentFunctionName.c_str());
              } else {
                  const std::string& baseModule = moduleIt->second.baseName;
                  MethodInfo inheritedMethod;
                  std::string inheritedOwner;
                  bool hasInheritedMethod = false;
                  if (!baseModule.empty()) {
                      hasInheritedMethod = resolveMethodInHierarchy(baseModule, currentFunctionName, $5, inheritedMethod, inheritedOwner);
                  }

                  if (currentClassMemberIsOverride && !hasInheritedMethod) {
                      printf("Semantic Error at line %d: method %s marked override but no base method exists\n", yylineno, currentFunctionName.c_str());
                  }

                  if (hasInheritedMethod && currentClassMemberIsOverride) {
                      if (inheritedMethod.arity != $5) {
                          printf("Semantic Error at line %d: override of %s has mismatched parameter count\n", yylineno, currentFunctionName.c_str());
                      }
                  }

                  MethodInfo methodInfo;
                  methodInfo.ownerModule = currentFunctionOwnerModule;
                  methodInfo.qualifiedName = currentFunctionQualifiedName;
                  methodInfo.parameterTypes = currentFunctionParamTypesInOrder;
                  methodInfo.returnType = $8;
                  methodInfo.isPublic = currentClassMemberIsPublic;
                  methodInfo.isOverride = currentClassMemberIsOverride;
                  methodInfo.isConstructor = currentFunctionIsConstructor;
                  methodInfo.arity = $5;
                  methodInfo.selector = buildSelector(currentFunctionName, $5);

                  std::vector<MethodInfo>& overloads = moduleIt->second.methods[currentFunctionName];
                  bool duplicateSignature = false;
                  for (const MethodInfo& existing : overloads) {
                      if (existing.arity == methodInfo.arity) {
                          duplicateSignature = true;
                          break;
                      }
                  }

                  if (duplicateSignature) {
                      printf("Semantic Error at line %d: method %s with %d parameters already defined in module %s\n", yylineno, currentFunctionName.c_str(), $5, currentFunctionOwnerModule.c_str());
                  } else {
                      overloads.push_back(methodInfo);
                      tacGenerator.registerMethod(currentFunctionOwnerModule, methodInfo.selector, currentFunctionQualifiedName);
                  }
              }
          } else {
              FunctionOverloadInfo overloadInfo;
              overloadInfo.qualifiedName = currentFunctionQualifiedName;
              overloadInfo.parameterTypes = currentFunctionParamTypesInOrder;
              overloadInfo.returnType = $8;
              overloadInfo.arity = $5;

              std::vector<FunctionOverloadInfo>& overloads = globalFunctionOverloads[currentFunctionName];
              bool duplicateSignature = false;
              for (const FunctionOverloadInfo& existing : overloads) {
                  if (existing.arity == overloadInfo.arity) {
                      duplicateSignature = true;
                      break;
                  }
              }

              if (duplicateSignature) {
                  printf("Semantic Error at line %d: function %s with %d parameters already defined\n", yylineno, currentFunctionName.c_str(), $5);
              } else {
                  overloads.push_back(overloadInfo);
              }
          }

          tacGenerator.emitFunctionBegin(currentFunctionQualifiedName);
          if (currentFunctionIsMethod) {
              tacGenerator.emitParamDef("this");
          }
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
                    tacGenerator.emitFunctionEnd(currentFunctionQualifiedName);
          currentFunctionParams.clear();
          currentFunctionParamTypes.clear();
                    currentFunctionParamTypesInOrder.clear();
          currentFunctionReturnType = "VOIDSPACE";
                    currentFunctionName.clear();
                    currentFunctionQualifiedName.clear();
                    currentFunctionOwnerModule.clear();
                    currentFunctionIsConstructor = false;
                    currentFunctionIsMethod = false;
          currentFunctionInModule = false;
                    currentFunctionSawValueReturn = false;
                    currentClassMemberIsOverride = false;
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
                    currentFunctionParamTypesInOrder.push_back($1);
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
    | NEW IDENTIFIER LPAREN argument_list_opt RPAREN
      {
          if (!isModuleKnown($2)) {
              printf("Semantic Error at line %d: module %s is not defined\n", yylineno, $2);
              $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
          } else {
              std::string objectTemp = tacGenerator.newTemp();
              tacGenerator.emitObjectNew($2, objectTemp);

              MethodInfo ctorInfo;
              std::string ctorOwner;
              if (resolveMethodInHierarchy($2, $2, $4, ctorInfo, ctorOwner)) {
                  if (ctorInfo.arity != $4) {
                      printf("Semantic Error at line %d: constructor %s expects %d arguments but got %d\n", yylineno, $2, static_cast<int>(ctorInfo.parameterTypes.size()), $4);
                  }
                  std::string initTemp = tacGenerator.emitMethodCall(objectTemp, ctorInfo.selector, $4);
                  (void)initTemp;
              } else if ($4 != 0) {
                  printf("Semantic Error at line %d: module %s has no constructor accepting %d arguments\n", yylineno, $2, $4);
              }

              $$ = makeExpr(objectTemp, 0.0, false, false, std::string("MODULE:") + $2);
          }
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
              $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
          } else {
              $$ = makeExpr("this", 0.0, false, true, std::string("MODULE:") + currentFunctionOwnerModule);
          }
      }
    | SUPER
      {
          if (!currentFunctionInModule) {
              printf("Semantic Error at line %d: super used outside module function context\n", yylineno);
              $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
          } else {
              auto moduleIt = moduleTable.find(currentFunctionOwnerModule);
              if (moduleIt == moduleTable.end() || moduleIt->second.baseName.empty()) {
                  printf("Semantic Error at line %d: super used in module without base\n", yylineno);
                  $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
              } else {
                  $$ = makeExpr("this", 0.0, false, true, std::string("MODULE:") + moduleIt->second.baseName);
              }
          }
      }
    | IDENTIFIER MEMBER_DOT IDENTIFIER LPAREN argument_list_opt RPAREN
      {
          if (!isDeclaredName($1)) {
              printf("Semantic Error at line %d: object %s not declared\n", yylineno, $1);
              $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
          } else {
              std::string objectType = declaredTypeOf($1);
              if (!isModuleType(objectType)) {
                  printf("Semantic Error at line %d: %s is not an object instance\n", yylineno, $1);
                  $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
              } else {
                  const std::string objectModule = moduleNameFromType(objectType);
                  MethodInfo methodInfo;
                  std::string ownerModule;
                  if (!resolveMethodInHierarchy(objectModule, $3, $5, methodInfo, ownerModule)) {
                      printf("Semantic Error at line %d: method %s not found in module %s\n", yylineno, $3, objectModule.c_str());
                      $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
                  } else if (!canAccessMember(methodInfo.isPublic, ownerModule)) {
                      printf("Semantic Error at line %d: method %s is private in module %s\n", yylineno, $3, ownerModule.c_str());
                      $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
                  } else {
                      if (methodInfo.arity != $5) {
                          printf("Semantic Error at line %d: method %s expects %d arguments but got %d\n", yylineno, $3, static_cast<int>(methodInfo.parameterTypes.size()), $5);
                      }
                      std::string callTemp = tacGenerator.emitMethodCall($1, methodInfo.selector, $5);
                      $$ = makeExpr(callTemp, 0.0, false, false, methodInfo.returnType);
                  }
              }
          }
      }
    | THIS MEMBER_DOT IDENTIFIER LPAREN argument_list_opt RPAREN
      {
          if (!currentFunctionInModule) {
              printf("Semantic Error at line %d: this used outside module function context\n", yylineno);
              $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
          } else {
              MethodInfo methodInfo;
              std::string ownerModule;
              if (!resolveMethodInHierarchy(currentFunctionOwnerModule, $3, $5, methodInfo, ownerModule)) {
                  printf("Semantic Error at line %d: method %s not found in module %s\n", yylineno, $3, currentFunctionOwnerModule.c_str());
                  $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
              } else if (!canAccessMember(methodInfo.isPublic, ownerModule)) {
                  printf("Semantic Error at line %d: method %s is private in module %s\n", yylineno, $3, ownerModule.c_str());
                  $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
              } else {
                  if (methodInfo.arity != $5) {
                      printf("Semantic Error at line %d: method %s expects %d arguments but got %d\n", yylineno, $3, static_cast<int>(methodInfo.parameterTypes.size()), $5);
                  }
                  std::string callTemp = tacGenerator.emitMethodCall("this", methodInfo.selector, $5);
                  $$ = makeExpr(callTemp, 0.0, false, false, methodInfo.returnType);
              }
          }
      }
    | SUPER MEMBER_DOT IDENTIFIER LPAREN argument_list_opt RPAREN
      {
          if (!currentFunctionInModule) {
              printf("Semantic Error at line %d: super used outside module function context\n", yylineno);
              $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
          } else {
              auto moduleIt = moduleTable.find(currentFunctionOwnerModule);
              if (moduleIt == moduleTable.end() || moduleIt->second.baseName.empty()) {
                  printf("Semantic Error at line %d: super used in module without base\n", yylineno);
                  $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
              } else {
                  MethodInfo methodInfo;
                  std::string ownerModule;
                  if (!resolveMethodInHierarchy(moduleIt->second.baseName, $3, $5, methodInfo, ownerModule)) {
                      printf("Semantic Error at line %d: method %s not found in base module chain\n", yylineno, $3);
                      $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
                  } else if (!canAccessMember(methodInfo.isPublic, ownerModule)) {
                      printf("Semantic Error at line %d: method %s is private in module %s\n", yylineno, $3, ownerModule.c_str());
                      $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
                  } else {
                      if (methodInfo.arity != $5) {
                          printf("Semantic Error at line %d: method %s expects %d arguments but got %d\n", yylineno, $3, static_cast<int>(methodInfo.parameterTypes.size()), $5);
                      }
                      std::string syntheticMethod = std::string("super:") + moduleIt->second.baseName + ":" + methodInfo.selector;
                      std::string callTemp = tacGenerator.emitMethodCall("this", syntheticMethod, $5);
                      $$ = makeExpr(callTemp, 0.0, false, false, methodInfo.returnType);
                  }
              }
          }
      }
    | IDENTIFIER MEMBER_DOT IDENTIFIER
      {
          if (!isDeclaredName($1)) {
              printf("Semantic Error at line %d: object %s not declared\n", yylineno, $1);
              $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
          } else {
              std::string objectType = declaredTypeOf($1);
              if (!isModuleType(objectType)) {
                  printf("Semantic Error at line %d: %s is not an object instance\n", yylineno, $1);
                  $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
              } else {
                  FieldInfo fieldInfo;
                  std::string ownerModule;
                  const std::string objectModule = moduleNameFromType(objectType);
                  if (!resolveFieldInHierarchy(objectModule, $3, fieldInfo, ownerModule)) {
                      printf("Semantic Error at line %d: field %s not found in module %s\n", yylineno, $3, objectModule.c_str());
                      $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
                  } else if (!canAccessMember(fieldInfo.isPublic, ownerModule)) {
                      printf("Semantic Error at line %d: field %s is private in module %s\n", yylineno, $3, ownerModule.c_str());
                      $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
                  } else {
                      std::string temp = tacGenerator.emitFieldLoad($1, $3);
                      $$ = makeExpr(temp, 0.0, false, false, fieldInfo.typeName);
                  }
              }
          }
      }
    | THIS MEMBER_DOT IDENTIFIER
      {
          if (!currentFunctionInModule) {
              printf("Semantic Error at line %d: this used outside module function context\n", yylineno);
              $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
          } else {
              FieldInfo fieldInfo;
              std::string ownerModule;
              if (!resolveFieldInHierarchy(currentFunctionOwnerModule, $3, fieldInfo, ownerModule)) {
                  printf("Semantic Error at line %d: field %s not found in module %s\n", yylineno, $3, currentFunctionOwnerModule.c_str());
                  $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
              } else if (!canAccessMember(fieldInfo.isPublic, ownerModule)) {
                  printf("Semantic Error at line %d: field %s is private in module %s\n", yylineno, $3, ownerModule.c_str());
                  $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
              } else {
                  std::string temp = tacGenerator.emitFieldLoad("this", $3);
                  $$ = makeExpr(temp, 0.0, false, false, fieldInfo.typeName);
              }
          }
      }
    | SUPER MEMBER_DOT IDENTIFIER
      {
          if (!currentFunctionInModule) {
              printf("Semantic Error at line %d: super used outside module function context\n", yylineno);
              $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
          } else {
              auto moduleIt = moduleTable.find(currentFunctionOwnerModule);
              if (moduleIt == moduleTable.end() || moduleIt->second.baseName.empty()) {
                  printf("Semantic Error at line %d: super used in module without base\n", yylineno);
                  $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
              } else {
                  FieldInfo fieldInfo;
                  std::string ownerModule;
                  if (!resolveFieldInHierarchy(moduleIt->second.baseName, $3, fieldInfo, ownerModule)) {
                      printf("Semantic Error at line %d: field %s not found in base module chain\n", yylineno, $3);
                      $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
                  } else if (!canAccessMember(fieldInfo.isPublic, ownerModule)) {
                      printf("Semantic Error at line %d: field %s is private in module %s\n", yylineno, $3, ownerModule.c_str());
                      $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
                  } else {
                      std::string temp = tacGenerator.emitFieldLoad("this", $3);
                      $$ = makeExpr(temp, 0.0, false, false, fieldInfo.typeName);
                  }
              }
          }
      }
    | IDENTIFIER
      {
          if (!isDeclaredName($1)) {
              if (currentFunctionInModule) {
                  FieldInfo fieldInfo;
                  std::string ownerModule;
                  if (resolveFieldInHierarchy(currentFunctionOwnerModule, $1, fieldInfo, ownerModule)) {
                      if (!canAccessMember(fieldInfo.isPublic, ownerModule)) {
                          printf("Semantic Error at line %d: field %s is private in module %s\n", yylineno, $1, ownerModule.c_str());
                          $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
                      } else {
                          std::string temp = tacGenerator.emitFieldLoad("this", $1);
                          $$ = makeExpr(temp, 0.0, false, false, fieldInfo.typeName);
                      }
                  } else {
                      printf("Semantic Error at line %d: variable %s not declared\n", yylineno, $1);
                      $$ = makeExpr($1, 0.0, false, false, "UNKNOWN");
                  }
              } else {
                  printf("Semantic Error at line %d: variable %s not declared\n", yylineno, $1);
                  $$ = makeExpr($1, 0.0, false, false, "UNKNOWN");
              }
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
          FunctionOverloadInfo overload;
          if (!resolveGlobalFunctionOverload($1, $3, overload)) {
              printf("Semantic Error at line %d: no overload of function %s accepts %d arguments\n", yylineno, $1, $3);
              $$ = makeExpr("0", 0.0, false, false, "UNKNOWN");
          } else {
              std::string callTemp = tacGenerator.emitCall(overload.qualifiedName, $3);
              $$ = makeExpr(callTemp, 0.0, false, false, overload.returnType);
          }
      }
    | LPAREN expression RPAREN
      {
          $$ = $2;
      }
    ;


%%

void yyerror(const char *s) {
    const char* tokenText = (yytext && yytext[0] != '\0') ? yytext : "<eof>";
    std::string hint = "Check statement terminators (.), bracket pairing, and token order.";

    if (strstr(s, "expecting DOT") != nullptr) {
        hint = "A statement is likely missing a terminating period '.' before this point.";
    } else if (strstr(s, "expecting RBRACE") != nullptr) {
        hint = "A block is missing a closing brace '}'.";
    } else if (strstr(s, "expecting RPAREN") != nullptr) {
        hint = "An expression or call is missing a closing parenthesis ')'.";
    } else if (strstr(s, "unexpected $end") != nullptr) {
        hint = "Input ended too early. Verify that mission/block braces and final 'success' are present.";
    } else if (strcmp(tokenText, "success") == 0) {
        hint = "The mission body may not be closed correctly before 'success'.";
    } else if (strcmp(tokenText, "}") == 0) {
        hint = "Unexpected '}'. Check for an extra closing brace or a missing statement before it.";
    }

    printf("SYNTAX ERROR at line %d: near '%s' - %s\n", yylineno, tokenText, hint.c_str());
}
