%option noyywrap

%option yylineno

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define PRINT_TOKEN(token) fprintf(yyout, "%s\n", token);
#define PRINT_TOKEN_VAL(token, val) fprintf(yyout, "%s %s\n", token, val);
%}



DIGIT           [0-9]
LETTER          [A-Za-z_]
IDENTIFIER      {LETTER}({LETTER}|{DIGIT})*
INT_LITERAL     {DIGIT}+
FLOAT_LITERAL   {DIGIT}+\.{DIGIT}+
STRING_LITERAL  \"([^\"\n])*\"


%%


"mission"          { PRINT_TOKEN("TOKEN_MISSION"); }
"launch"           { PRINT_TOKEN("TOKEN_LAUNCH"); }
"success"          { PRINT_TOKEN("TOKEN_SUCCESS"); }
"abort"            { PRINT_TOKEN("TOKEN_ABORT"); }

"module"           { PRINT_TOKEN("TOKEN_MODULE"); }
"deploy"           { PRINT_TOKEN("TOKEN_DEPLOY"); }
"extends"          { PRINT_TOKEN("TOKEN_EXTENDS"); }
"public"           { PRINT_TOKEN("TOKEN_PUBLIC"); }
"private"          { PRINT_TOKEN("TOKEN_PRIVATE"); }
"this"             { PRINT_TOKEN("TOKEN_THIS"); }


"telemetry"        { PRINT_TOKEN("TOKEN_TELEMETRY"); }
"limit"            { PRINT_TOKEN("TOKEN_LIMIT"); }
"alias"            { PRINT_TOKEN("TOKEN_ALIAS"); }
"fleet"            { PRINT_TOKEN("TOKEN_FLEET"); }
"mode"             { PRINT_TOKEN("TOKEN_MODE"); }


"command"          { PRINT_TOKEN("TOKEN_COMMAND"); }
"back"             { PRINT_TOKEN("TOKEN_BACK"); }


"verify"           { PRINT_TOKEN("TOKEN_VERIFY"); }
"else_verify"      { PRINT_TOKEN("TOKEN_ELSE_VERIFY"); }
"otherwise"        { PRINT_TOKEN("TOKEN_OTHERWISE"); }

"orbit"            { PRINT_TOKEN("TOKEN_ORBIT"); }
"scenario"         { PRINT_TOKEN("TOKEN_SCENARIO"); }
"trajectory"       { PRINT_TOKEN("TOKEN_TRAJECTORY"); }
"fallback"         { PRINT_TOKEN("TOKEN_FALLBACK"); }

"stage_sep"        { PRINT_TOKEN("TOKEN_STAGE_SEP"); }
"coast"            { PRINT_TOKEN("TOKEN_COAST"); }


"wait"             { PRINT_TOKEN("TOKEN_WAIT"); }
"tick"             { PRINT_TOKEN("TOKEN_TICK"); }


"count"            { PRINT_TOKEN("TOKEN_COUNT"); }
"real"             { PRINT_TOKEN("TOKEN_REAL"); }
"precise"          { PRINT_TOKEN("TOKEN_PRECISE"); }
"flag"             { PRINT_TOKEN("TOKEN_FLAG"); }
"symbol"           { PRINT_TOKEN("TOKEN_SYMBOL"); }
"voidspace"        { PRINT_TOKEN("TOKEN_VOIDSPACE"); }


"add"              { PRINT_TOKEN("TOKEN_ADD"); }
"minus"            { PRINT_TOKEN("TOKEN_MINUS"); }
"mul"              { PRINT_TOKEN("TOKEN_MUL"); }
"divide"           { PRINT_TOKEN("TOKEN_DIVIDE"); }
"mod"              { PRINT_TOKEN("TOKEN_MOD"); }
"**"               { PRINT_TOKEN("TOKEN_POWER"); }


"AND"              { PRINT_TOKEN("TOKEN_AND"); }
"OR"               { PRINT_TOKEN("TOKEN_OR"); }
"NOT"              { PRINT_TOKEN("TOKEN_NOT"); }
"XOR"              { PRINT_TOKEN("TOKEN_XOR"); }

"<="               { PRINT_TOKEN("TOKEN_LE"); }
">="               { PRINT_TOKEN("TOKEN_GE"); }
"=="               { PRINT_TOKEN("TOKEN_EQ"); }
"!="               { PRINT_TOKEN("TOKEN_NEQ"); }
"<"                { PRINT_TOKEN("TOKEN_LT"); }
">"                { PRINT_TOKEN("TOKEN_GT"); }


":="               { PRINT_TOKEN("TOKEN_ASSIGN"); }


"{"                { PRINT_TOKEN("TOKEN_LBRACE"); }
"}"                { PRINT_TOKEN("TOKEN_RBRACE"); }
"("                { PRINT_TOKEN("TOKEN_LPAREN"); }
")"                { PRINT_TOKEN("TOKEN_RPAREN"); }
"["                { PRINT_TOKEN("TOKEN_LBRACKET"); }
"]"                { PRINT_TOKEN("TOKEN_RBRACKET"); }
"."                { PRINT_TOKEN("TOKEN_DOT"); }
","                { PRINT_TOKEN("TOKEN_COMMA"); }
":"                { PRINT_TOKEN("TOKEN_COLON"); }

{FLOAT_LITERAL}    { PRINT_TOKEN_VAL("TOKEN_FLOAT_LITERAL", yytext); }
{INT_LITERAL}      { PRINT_TOKEN_VAL("TOKEN_INT_LITERAL", yytext); }
{STRING_LITERAL}   { PRINT_TOKEN_VAL("TOKEN_STRING_LITERAL", yytext); }


{IDENTIFIER}       { PRINT_TOKEN_VAL("TOKEN_IDENTIFIER", yytext); }


"\$\$".*           { /* Ignore */ }

"\$\*"([^*]|\*+[^$])*\*+"\$"   { /* Ignore */ }

[ \t\r]+           { /* Ignore spaces and tabs */ }
\n                 { /* Line numbers auto-tracked */ }


. {
    fprintf(stderr,
            "LEXICAL ERROR at line %d: Unknown symbol '%s'\n",
            yylineno, yytext);
}

%%

int main(int argc, char* argv[]) {

    if (argc < 3) {
        printf("Usage: ./lexer <input_file> <output_file>\n");
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        printf("Cannot open input file.\n");
        return 1;
    }

    yyout = fopen(argv[2], "w");
    if (!yyout) {
        printf("Cannot open output file.\n");
        return 1;
    }

    yylex();

    fclose(yyin);
    fclose(yyout);

    return 0;
}
