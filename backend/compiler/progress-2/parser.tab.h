
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
     BROADCAST = 275,
     RECEIVE = 276,
     ALARM = 277,
     VERIFY = 278,
     ELSE_VERIFY = 279,
     OTHERWISE = 280,
     ORBIT = 281,
     SCENARIO = 282,
     TRAJECTORY = 283,
     FALLBACK = 284,
     STAGE_SEP = 285,
     COAST = 286,
     WAIT = 287,
     TICK = 288,
     ROOT = 289,
     FLR = 290,
     CEIL = 291,
     ABS = 292,
     LOGARITHM = 293,
     SINE = 294,
     COSINE = 295,
     TAN = 296,
     ASINE = 297,
     ACOSINE = 298,
     ATAN = 299,
     PRIME = 300,
     WHILE = 301,
     TIMES = 302,
     COUNT = 303,
     REAL = 304,
     PRECISE = 305,
     FLAG = 306,
     SYMBOL = 307,
     VOIDSPACE = 308,
     ADD = 309,
     MINUS = 310,
     MUL = 311,
     DIVIDE = 312,
     MOD = 313,
     POWER = 314,
     AND = 315,
     OR = 316,
     NOT = 317,
     XOR = 318,
     LT = 319,
     GT = 320,
     LE = 321,
     GE = 322,
     EQ = 323,
     NEQ = 324,
     ASSIGN = 325,
     RETTYPE = 326,
     LBRACKET = 327,
     RBRACKET = 328,
     LBRACE = 329,
     RBRACE = 330,
     LPAREN = 331,
     RPAREN = 332,
     DOT = 333,
     COMMA = 334,
     COLON = 335,
     INT_LITERAL = 336,
     FLOAT_LITERAL = 337,
     IDENTIFIER = 338,
     STRING_LITERAL = 339,
     LOWER_THAN_ELSE = 340
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 13 "parser.y"

    int intval;
    double floatval;
    char* str;



/* Line 1676 of yacc.c  */
#line 145 "parser.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


