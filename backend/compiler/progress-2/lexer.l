%option noyywrap
%option yylineno

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"

extern YYSTYPE yylval;
%}

DIGIT           [0-9]
LETTER          [A-Za-z_]
IDENTIFIER      {LETTER}({LETTER}|{DIGIT})*
INT_LITERAL     {DIGIT}+
FLOAT_LITERAL   {DIGIT}+\.{DIGIT}+
STRING_LITERAL  \"([^\"\n])*\"


%%

"mission"          { return MISSION; }
"launch"           { return LAUNCH; }
"success"          { return SUCCESS; }
"abort"            { return ABORT; }

"module"           { return MODULE; }
"deploy"           { return DEPLOY; }
"extends"          { return EXTENDS; }
"public"           { return PUBLIC; }
"private"          { return PRIVATE; }
"this"             { return THIS; }

"telemetry"        { return TELEMETRY; }
"limit"            { return LIMIT; }
"alias"            { return ALIAS; }
"fleet"            { return FLEET; }
"mode"             { return MODE; }

"command"          { return COMMAND; }
"back"             { return BACK; }

"verify"           { return VERIFY; }
"else_verify"      { return ELSE_VERIFY; }
"otherwise"        { return OTHERWISE; }

"orbit"            { return ORBIT; }
"scenario"         { return SCENARIO; }
"trajectory"       { return TRAJECTORY; }
"fallback"         { return FALLBACK; }

"stage_sep"        { return STAGE_SEP; }
"coast"            { return COAST; }

"wait"             { return WAIT; }
"tick"             { return TICK; }

"count"            { return COUNT; }
"real"             { return REAL; }
"precise"          { return PRECISE; }
"flag"             { return FLAG; }
"symbol"           { return SYMBOL; }
"voidspace"        { return VOIDSPACE; }

"add"              { return ADD; }
"minus"            { return MINUS; }
"mul"              { return MUL; }
"divide"           { return DIVIDE; }
"mod"              { return MOD; }
"**"               { return POWER; }

"AND"              { return AND; }
"OR"               { return OR; }
"NOT"              { return NOT; }
"XOR"              { return XOR; }

"<="               { return LE; }
">="               { return GE; }
"=="               { return EQ; }
"!="               { return NEQ; }
"<"                { return LT; }
">"                { return GT; }

":="               { return ASSIGN; }

"{"                { return LBRACE; }
"}"                { return RBRACE; }
"("                { return LPAREN; }
")"                { return RPAREN; }
"["                { return LBRACKET; }
"]"                { return RBRACKET; }
"."                { return DOT; }
","                { return COMMA; }
":"                { return COLON; }

{FLOAT_LITERAL}    {
                      yylval.floatval = atof(yytext);
                      return FLOAT_LITERAL;
                   }

{INT_LITERAL}      {
                      yylval.intval = atoi(yytext);
                      return INT_LITERAL;
                   }

{STRING_LITERAL}   {
                      yylval.str = strdup(yytext);
                      return STRING_LITERAL;
                   }

{IDENTIFIER}       {
                      yylval.str = strdup(yytext);
                      return IDENTIFIER;
                   }

"\$\$".*           { }

"\$\*"([^*]|\*+[^$])*\*+"\$"   { }

[ \t\r\n]+         { }

. {
      printf("LEXICAL ERROR at line %d: %s\n", yylineno, yytext);
  }

%%
