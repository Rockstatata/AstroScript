
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     MISSION = 258,
     LAUNCH = 259,
     SUCCESS = 260,
     ABORT = 261,
     MODULE = 262,
     DEPLOY = 263,
     EXTENDS = 264,
     PUBLIC = 265,
     PRIVATE = 266,
     THIS = 267,
     TELEMETRY = 268,
     LIMIT = 269,
     ALIAS = 270,
     FLEET = 271,
     MODE = 272,
     COMMAND = 273,
     BACK = 274,
     VERIFY = 275,
     ELSE_VERIFY = 276,
     OTHERWISE = 277,
     ORBIT = 278,
     SCENARIO = 279,
     TRAJECTORY = 280,
     FALLBACK = 281,
     STAGE_SEP = 282,
     COAST = 283,
     WAIT = 284,
     TICK = 285,
     COUNT = 286,
     REAL = 287,
     PRECISE = 288,
     FLAG = 289,
     SYMBOL = 290,
     VOIDSPACE = 291,
     ADD = 292,
     MINUS = 293,
     MUL = 294,
     DIVIDE = 295,
     MOD = 296,
     POWER = 297,
     AND = 298,
     OR = 299,
     NOT = 300,
     XOR = 301,
     LT = 302,
     GT = 303,
     LE = 304,
     GE = 305,
     EQ = 306,
     NEQ = 307,
     ASSIGN = 308,
     LBRACKET = 309,
     RBRACKET = 310,
     LBRACE = 311,
     RBRACE = 312,
     LPAREN = 313,
     RPAREN = 314,
     DOT = 315,
     COMMA = 316,
     COLON = 317,
     INT_LITERAL = 318,
     FLOAT_LITERAL = 319,
     IDENTIFIER = 320,
     STRING_LITERAL = 321,
     LOWER_THAN_ELSE = 322
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 11 "parser.y"

    int intval;
    double floatval;
    char* str;



/* Line 1676 of yacc.c  */
#line 127 "parser.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


