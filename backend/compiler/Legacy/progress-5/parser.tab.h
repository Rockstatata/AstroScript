
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
     TRANSMIT = 275,
     BROADCAST = 276,
     RECEIVE = 277,
     ALARM = 278,
     VERIFY = 279,
     ELSE_VERIFY = 280,
     OTHERWISE = 281,
     ORBIT = 282,
     SCENARIO = 283,
     TRAJECTORY = 284,
     FALLBACK = 285,
     STAGE_SEP = 286,
     COAST = 287,
     WAIT = 288,
     TICK = 289,
     ROOT = 290,
     FLR = 291,
     CEIL = 292,
     ABS = 293,
     LOGARITHM = 294,
     SINE = 295,
     COSINE = 296,
     TAN = 297,
     ASINE = 298,
     ACOSINE = 299,
     ATAN = 300,
     PRIME = 301,
     WHILE = 302,
     TIMES = 303,
     COUNT = 304,
     REAL = 305,
     PRECISE = 306,
     FLAG = 307,
     SYMBOL = 308,
     VOIDSPACE = 309,
     ADD = 310,
     MINUS = 311,
     MUL = 312,
     DIVIDE = 313,
     MOD = 314,
     POWER = 315,
     AND = 316,
     OR = 317,
     NOT = 318,
     XOR = 319,
     LT = 320,
     GT = 321,
     LE = 322,
     GE = 323,
     EQ = 324,
     NEQ = 325,
     ASSIGN = 326,
     RETTYPE = 327,
     LBRACKET = 328,
     RBRACKET = 329,
     LBRACE = 330,
     RBRACE = 331,
     LPAREN = 332,
     RPAREN = 333,
     DOT = 334,
     COMMA = 335,
     COLON = 336,
     INT_LITERAL = 337,
     FLOAT_LITERAL = 338,
     IDENTIFIER = 339,
     STRING_LITERAL = 340,
     LOWER_THAN_ELSE = 341
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 18 "parser.y"

    int intval;
    double floatval;
    char* str;



/* Line 1676 of yacc.c  */
#line 146 "parser.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


