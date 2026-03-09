
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "parser.y"

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


/* Line 189 of yacc.c  */
#line 155 "parser.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

/* "%code requires" blocks.  */

/* Line 209 of yacc.c  */
#line 82 "parser.y"

    struct ExprAttr;



/* Line 209 of yacc.c  */
#line 185 "parser.tab.c"

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

/* Line 214 of yacc.c  */
#line 87 "parser.y"

    int intval;
    double floatval;
    char* str;
    ExprAttr* expr;



/* Line 214 of yacc.c  */
#line 297 "parser.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 309 "parser.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   512

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  87
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  48
/* YYNRULES -- Number of rules.  */
#define YYNRULES  111
/* YYNRULES -- Number of states.  */
#define YYNSTATES  236

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   341

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,    11,    15,    18,    19,    21,    23,
      25,    27,    30,    32,    34,    36,    38,    40,    43,    46,
      48,    51,    56,    63,    70,    75,    80,    86,    94,   104,
     109,   117,   121,   125,   132,   135,   139,   142,   143,   147,
     151,   153,   155,   157,   159,   161,   163,   164,   165,   175,
     176,   177,   186,   187,   190,   191,   192,   193,   201,   202,
     203,   212,   213,   225,   226,   235,   238,   239,   240,   246,
     247,   252,   257,   264,   267,   268,   271,   272,   274,   277,
     280,   282,   283,   284,   295,   297,   299,   301,   302,   306,
     308,   311,   313,   314,   316,   320,   324,   328,   332,   336,
     340,   344,   348,   352,   356,   360,   362,   364,   366,   368,
     373,   378
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      88,     0,    -1,    89,     5,    -1,     3,    84,     4,    90,
      -1,    75,    91,    76,    -1,    91,    92,    -1,    -1,    93,
      -1,    94,    -1,    95,    -1,    96,    -1,   134,    79,    -1,
     101,    -1,   108,    -1,   114,    -1,   121,    -1,   120,    -1,
      31,    79,    -1,    32,    79,    -1,    97,    -1,     6,    79,
      -1,    13,   100,    84,    79,    -1,    13,   100,    84,    71,
     134,    79,    -1,    14,   100,    84,    71,   134,    79,    -1,
      15,   100,    84,    79,    -1,    16,   100,    84,    79,    -1,
      17,    84,    75,    98,    76,    -1,    13,   100,    84,    73,
      82,    74,    79,    -1,    13,   100,    84,    73,    82,    74,
      71,   134,    79,    -1,    84,    71,   134,    79,    -1,    84,
      73,   134,    74,    71,   134,    79,    -1,    20,   134,    79,
      -1,    22,    84,    79,    -1,    22,    84,    73,   134,    74,
      79,    -1,    19,    79,    -1,    19,   134,    79,    -1,    98,
      99,    -1,    -1,    29,    82,    79,    -1,    30,    82,    79,
      -1,    49,    -1,    50,    -1,    51,    -1,    52,    -1,    53,
      -1,    54,    -1,    -1,    -1,    24,    77,   134,    78,   102,
      90,   103,   104,   107,    -1,    -1,    -1,    25,    77,   134,
      78,   105,    90,   106,   104,    -1,    -1,    26,    90,    -1,
      -1,    -1,    -1,    27,    77,   109,   134,    78,   110,    90,
      -1,    -1,    -1,    27,    47,    77,   111,   134,    78,   112,
      90,    -1,    -1,    27,    48,    77,   134,    81,   134,    81,
     134,    78,   113,    90,    -1,    -1,    28,    77,   134,    78,
     115,    75,   116,    76,    -1,   116,   117,    -1,    -1,    -1,
      29,    82,    81,   118,    91,    -1,    -1,    30,    81,   119,
      91,    -1,    33,    82,    34,    79,    -1,     7,    84,   122,
      75,   123,    76,    -1,     9,    84,    -1,    -1,   123,   124,
      -1,    -1,    93,    -1,    10,    93,    -1,    11,    93,    -1,
     125,    -1,    -1,    -1,    18,    84,    77,   126,   129,    78,
     128,   100,   127,    90,    -1,    81,    -1,    72,    -1,   130,
      -1,    -1,   130,    80,   131,    -1,   131,    -1,   100,    84,
      -1,   133,    -1,    -1,   134,    -1,   133,    80,   134,    -1,
     134,    65,   134,    -1,   134,    66,   134,    -1,   134,    67,
     134,    -1,   134,    68,   134,    -1,   134,    69,   134,    -1,
     134,    70,   134,    -1,   134,    55,   134,    -1,   134,    56,
     134,    -1,   134,    57,   134,    -1,   134,    58,   134,    -1,
      82,    -1,    83,    -1,    85,    -1,    84,    -1,    84,    73,
     134,    74,    -1,    84,    77,   132,    78,    -1,    77,   134,
      78,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   137,   137,   141,   145,   149,   150,   154,   155,   156,
     157,   158,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   174,   183,   194,   205,   206,   207,   208,   217,   231,
     240,   253,   261,   269,   283,   287,   295,   296,   300,   301,
     305,   306,   307,   308,   309,   310,   315,   322,   314,   337,
     345,   336,   351,   355,   356,   361,   367,   360,   380,   386,
     379,   399,   398,   425,   424,   442,   443,   448,   447,   463,
     462,   475,   482,   486,   487,   491,   492,   496,   497,   498,
     499,   504,   509,   503,   525,   526,   530,   531,   535,   539,
     546,   553,   554,   558,   564,   573,   580,   587,   594,   601,
     608,   615,   622,   629,   636,   643,   647,   651,   655,   664,
     675,   680
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "MISSION", "LAUNCH", "SUCCESS", "ABORT",
  "MODULE", "DEPLOY", "EXTENDS", "PUBLIC", "PRIVATE", "THIS", "TELEMETRY",
  "LIMIT", "ALIAS", "FLEET", "MODE", "COMMAND", "BACK", "TRANSMIT",
  "BROADCAST", "RECEIVE", "ALARM", "VERIFY", "ELSE_VERIFY", "OTHERWISE",
  "ORBIT", "SCENARIO", "TRAJECTORY", "FALLBACK", "STAGE_SEP", "COAST",
  "WAIT", "TICK", "ROOT", "FLR", "CEIL", "ABS", "LOGARITHM", "SINE",
  "COSINE", "TAN", "ASINE", "ACOSINE", "ATAN", "PRIME", "WHILE", "TIMES",
  "COUNT", "REAL", "PRECISE", "FLAG", "SYMBOL", "VOIDSPACE", "ADD",
  "MINUS", "MUL", "DIVIDE", "MOD", "POWER", "AND", "OR", "NOT", "XOR",
  "LT", "GT", "LE", "GE", "EQ", "NEQ", "ASSIGN", "RETTYPE", "LBRACKET",
  "RBRACKET", "LBRACE", "RBRACE", "LPAREN", "RPAREN", "DOT", "COMMA",
  "COLON", "INT_LITERAL", "FLOAT_LITERAL", "IDENTIFIER", "STRING_LITERAL",
  "LOWER_THAN_ELSE", "$accept", "program", "mission_block", "block",
  "statement_list", "statement", "declaration", "assignment", "print_stmt",
  "input_stmt", "return_statement", "mode_body", "mode_entry", "type",
  "control_statement", "$@1", "$@2", "else_verify_chain", "$@3", "$@4",
  "otherwise_clause", "loop_statement", "$@5", "$@6", "$@7", "$@8", "$@9",
  "switch_statement", "$@10", "case_list", "case_entry", "$@11", "$@12",
  "wait_statement", "module_definition", "inheritance", "class_body",
  "class_member", "function_definition", "$@13", "$@14", "return_type",
  "parameter_list_opt", "parameter_list", "parameter", "argument_list_opt",
  "argument_list", "expression", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    87,    88,    89,    90,    91,    91,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    93,    93,    93,    93,    93,    93,    93,    93,    94,
      94,    95,    96,    96,    97,    97,    98,    98,    99,    99,
     100,   100,   100,   100,   100,   100,   102,   103,   101,   105,
     106,   104,   104,   107,   107,   109,   110,   108,   111,   112,
     108,   113,   108,   115,   114,   116,   116,   118,   117,   119,
     117,   120,   121,   122,   122,   123,   123,   124,   124,   124,
     124,   126,   127,   125,   128,   128,   129,   129,   130,   130,
     131,   132,   132,   133,   133,   134,   134,   134,   134,   134,
     134,   134,   134,   134,   134,   134,   134,   134,   134,   134,
     134,   134
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     4,     3,     2,     0,     1,     1,     1,
       1,     2,     1,     1,     1,     1,     1,     2,     2,     1,
       2,     4,     6,     6,     4,     4,     5,     7,     9,     4,
       7,     3,     3,     6,     2,     3,     2,     0,     3,     3,
       1,     1,     1,     1,     1,     1,     0,     0,     9,     0,
       0,     8,     0,     2,     0,     0,     0,     7,     0,     0,
       8,     0,    11,     0,     8,     2,     0,     0,     5,     0,
       4,     4,     6,     2,     0,     2,     0,     1,     2,     2,
       1,     0,     0,    10,     1,     1,     1,     0,     3,     1,
       2,     1,     0,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     1,     1,     1,     1,     4,
       4,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     1,     2,     0,     6,     3,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     4,     0,   105,
     106,   108,   107,     5,     7,     8,     9,    10,    19,    12,
      13,    14,    16,    15,     0,    20,    74,    40,    41,    42,
      43,    44,    45,     0,     0,     0,     0,     0,    34,   108,
       0,     0,     0,     0,     0,     0,    55,     0,    17,    18,
       0,     0,     0,     0,    92,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    11,     0,     0,     0,     0,
       0,     0,    37,     0,    35,    31,     0,    32,     0,    58,
       0,     0,     0,     0,   111,     0,     0,     0,    91,    93,
     101,   102,   103,   104,    95,    96,    97,    98,    99,   100,
      73,    76,     0,     0,    21,     0,    24,    25,     0,     0,
       0,    46,     0,     0,     0,    63,    71,    29,   109,   110,
       0,     0,     0,     0,     0,     0,     0,    26,    36,   109,
       0,     0,     0,     0,    56,     0,     0,    94,     0,     0,
       0,    72,    77,    75,    80,    22,     0,    23,     0,     0,
      33,    47,    59,     0,     0,    66,     0,    78,    79,     0,
       0,    27,    38,    39,    52,     0,     0,    57,     0,    30,
      81,     0,     0,    54,    60,     0,     0,     0,    64,    65,
      87,    28,     0,     0,    48,    61,     0,    69,     0,     0,
      86,    89,     0,    53,     0,    67,     6,    90,     0,     0,
      49,    62,     6,    70,    85,    84,     0,    88,     0,    68,
      82,    50,     0,    52,    83,    51
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,     9,    10,    33,    34,    35,    36,    37,
      38,   128,   148,   208,    39,   151,   184,   193,   228,   233,
     204,    40,   101,   174,   132,   185,   214,    41,   155,   188,
     199,   222,   216,    42,    43,    87,   141,   163,   164,   200,
     232,   226,   209,   210,   211,   107,   108,    44
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -146
static const yytype_int16 yypact[] =
{
      10,   -67,    31,    29,    41,  -146,  -146,     0,  -146,  -146,
     100,   -32,    -6,   412,   412,   412,   412,     1,    11,    52,
       5,    21,   -41,    22,    30,    33,    18,  -146,    52,  -146,
    -146,     3,  -146,  -146,  -146,  -146,  -146,  -146,  -146,  -146,
    -146,  -146,  -146,  -146,   -28,  -146,   101,  -146,  -146,  -146,
    -146,  -146,  -146,    37,    39,    46,    54,    50,  -146,   -61,
     174,   190,   -27,    52,    63,    64,  -146,    52,  -146,  -146,
     121,   140,    52,    52,    52,    52,    52,    52,    52,    52,
      52,    52,    52,    52,    52,  -146,    74,    85,   -23,    91,
      84,    88,  -146,    52,  -146,  -146,    52,  -146,   286,  -146,
      52,    52,   302,    92,  -146,   206,   382,    95,    94,   442,
      34,    34,  -146,  -146,    47,    47,    47,    47,    47,    47,
    -146,  -146,    52,    93,  -146,    52,  -146,  -146,   -21,   402,
     422,  -146,    52,   134,   318,  -146,  -146,  -146,   107,  -146,
      52,     8,   222,   105,   238,    98,    99,  -146,  -146,  -146,
     108,     0,   334,    52,  -146,   111,    52,   442,   132,   132,
     109,  -146,  -146,  -146,  -146,  -146,     2,  -146,   115,   133,
    -146,  -146,  -146,   168,     0,  -146,   254,  -146,  -146,   139,
      52,  -146,  -146,  -146,   186,     0,    52,  -146,   -19,  -146,
    -146,   270,   150,   191,  -146,   350,   146,   169,  -146,  -146,
     412,  -146,    52,     0,  -146,  -146,   170,  -146,   181,   176,
     172,  -146,   366,  -146,     0,  -146,  -146,  -146,   -37,   412,
    -146,  -146,  -146,   137,  -146,  -146,   412,  -146,     0,   137,
    -146,  -146,     0,   186,  -146,  -146
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -146,  -146,  -146,  -131,  -145,  -146,  -126,  -146,  -146,  -146,
    -146,  -146,  -146,   -13,  -146,  -146,  -146,    35,  -146,  -146,
    -146,  -146,  -146,  -146,  -146,  -146,  -146,  -146,  -146,  -146,
    -146,  -146,  -146,  -146,  -146,  -146,  -146,  -146,  -146,  -146,
    -146,  -146,  -146,  -146,    48,  -146,  -146,   -14
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      53,    54,    55,    56,    60,    61,    64,    65,   145,   146,
     196,   197,    93,     1,    71,   162,    74,     4,   158,   159,
     171,    13,    14,    15,    16,    17,   160,    75,    76,    77,
      78,     5,   177,   178,     6,   224,    66,    79,    80,    81,
      82,    83,    84,   187,   225,     7,    96,    45,   122,    98,
     123,    85,    97,   102,   194,   147,   124,   198,   105,   106,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   223,   213,   180,    72,     8,    73,   229,    46,   129,
      74,   181,   130,   221,   161,    57,   133,   134,    28,    62,
      58,    77,    78,    29,    30,    59,    32,   231,    63,    67,
      70,   234,    75,    76,    77,    78,    11,    12,   142,    68,
      86,   144,    69,    13,    14,    15,    16,    17,   152,    18,
      19,    88,    20,    89,    21,    92,   157,    22,    23,    28,
      90,    24,    25,    26,    29,    30,    59,    32,    91,   173,
      99,   100,   176,    11,    12,    13,    14,    15,    16,    17,
      13,    14,    15,    16,    17,   103,    18,    19,   120,    20,
     121,    21,   125,   126,    22,    23,   191,   127,    24,    25,
      26,   136,   195,   139,   140,   143,    27,    28,   156,   166,
     168,   169,    29,    30,    31,    32,   175,   170,   212,    75,
      76,    77,    78,   179,   182,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    79,    80,    81,    82,    83,
      84,   192,   183,   230,    28,   153,   190,   203,   104,    29,
      30,    31,    32,    75,    76,    77,    78,   202,   206,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    79,
      80,    81,    82,    83,    84,    75,    76,    77,    78,   186,
     207,   215,   219,    94,   218,    79,    80,    81,    82,    83,
      84,    75,    76,    77,    78,   217,     0,   227,   235,    95,
       0,    79,    80,    81,    82,    83,    84,    75,    76,    77,
      78,     0,     0,     0,     0,   137,     0,    79,    80,    81,
      82,    83,    84,    75,    76,    77,    78,     0,     0,     0,
       0,   165,     0,    79,    80,    81,    82,    83,    84,    75,
      76,    77,    78,     0,     0,     0,     0,   167,     0,    79,
      80,    81,    82,    83,    84,    75,    76,    77,    78,     0,
       0,     0,     0,   189,     0,    79,    80,    81,    82,    83,
      84,    75,    76,    77,    78,     0,     0,     0,     0,   201,
       0,    79,    80,    81,    82,    83,    84,    75,    76,    77,
      78,     0,     0,     0,   131,     0,     0,    79,    80,    81,
      82,    83,    84,    75,    76,    77,    78,     0,     0,     0,
     135,     0,     0,    79,    80,    81,    82,    83,    84,    75,
      76,    77,    78,     0,     0,     0,   154,     0,     0,    79,
      80,    81,    82,    83,    84,    75,    76,    77,    78,     0,
       0,     0,   172,     0,     0,    79,    80,    81,    82,    83,
      84,    75,    76,    77,    78,     0,     0,     0,   205,     0,
       0,    79,    80,    81,    82,    83,    84,    75,    76,    77,
      78,     0,     0,     0,   220,     0,     0,    79,    80,    81,
      82,    83,    84,     0,     0,     0,   138,    75,    76,    77,
      78,    47,    48,    49,    50,    51,    52,    79,    80,    81,
      82,    83,    84,     0,     0,     0,   149,    75,    76,    77,
      78,     0,     0,     0,     0,     0,     0,    79,    80,    81,
      82,    83,    84,     0,     0,     0,   150,    75,    76,    77,
      78,     0,     0,     0,     0,     0,     0,    79,    80,    81,
      82,    83,    84
};

static const yytype_int16 yycheck[] =
{
      13,    14,    15,    16,    18,    19,    47,    48,    29,    30,
      29,    30,    73,     3,    28,   141,    77,    84,    10,    11,
     151,    13,    14,    15,    16,    17,    18,    55,    56,    57,
      58,     0,   158,   159,     5,    72,    77,    65,    66,    67,
      68,    69,    70,   174,    81,     4,    73,    79,    71,    63,
      73,    79,    79,    67,   185,    76,    79,    76,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,   216,   203,    71,    71,    75,    73,   222,    84,    93,
      77,    79,    96,   214,    76,    84,   100,   101,    77,    84,
      79,    57,    58,    82,    83,    84,    85,   228,    77,    77,
      82,   232,    55,    56,    57,    58,     6,     7,   122,    79,
       9,   125,    79,    13,    14,    15,    16,    17,   132,    19,
      20,    84,    22,    84,    24,    75,   140,    27,    28,    77,
      84,    31,    32,    33,    82,    83,    84,    85,    84,   153,
      77,    77,   156,     6,     7,    13,    14,    15,    16,    17,
      13,    14,    15,    16,    17,    34,    19,    20,    84,    22,
      75,    24,    71,    79,    27,    28,   180,    79,    31,    32,
      33,    79,   186,    78,    80,    82,    76,    77,    71,    74,
      82,    82,    82,    83,    84,    85,    75,    79,   202,    55,
      56,    57,    58,    84,    79,    55,    56,    57,    58,    65,
      66,    67,    68,    69,    70,    65,    66,    67,    68,    69,
      70,    25,    79,   226,    77,    81,    77,    26,    78,    82,
      83,    84,    85,    55,    56,    57,    58,    77,    82,    55,
      56,    57,    58,    65,    66,    67,    68,    69,    70,    65,
      66,    67,    68,    69,    70,    55,    56,    57,    58,    81,
      81,    81,    80,    79,    78,    65,    66,    67,    68,    69,
      70,    55,    56,    57,    58,    84,    -1,   219,   233,    79,
      -1,    65,    66,    67,    68,    69,    70,    55,    56,    57,
      58,    -1,    -1,    -1,    -1,    79,    -1,    65,    66,    67,
      68,    69,    70,    55,    56,    57,    58,    -1,    -1,    -1,
      -1,    79,    -1,    65,    66,    67,    68,    69,    70,    55,
      56,    57,    58,    -1,    -1,    -1,    -1,    79,    -1,    65,
      66,    67,    68,    69,    70,    55,    56,    57,    58,    -1,
      -1,    -1,    -1,    79,    -1,    65,    66,    67,    68,    69,
      70,    55,    56,    57,    58,    -1,    -1,    -1,    -1,    79,
      -1,    65,    66,    67,    68,    69,    70,    55,    56,    57,
      58,    -1,    -1,    -1,    78,    -1,    -1,    65,    66,    67,
      68,    69,    70,    55,    56,    57,    58,    -1,    -1,    -1,
      78,    -1,    -1,    65,    66,    67,    68,    69,    70,    55,
      56,    57,    58,    -1,    -1,    -1,    78,    -1,    -1,    65,
      66,    67,    68,    69,    70,    55,    56,    57,    58,    -1,
      -1,    -1,    78,    -1,    -1,    65,    66,    67,    68,    69,
      70,    55,    56,    57,    58,    -1,    -1,    -1,    78,    -1,
      -1,    65,    66,    67,    68,    69,    70,    55,    56,    57,
      58,    -1,    -1,    -1,    78,    -1,    -1,    65,    66,    67,
      68,    69,    70,    -1,    -1,    -1,    74,    55,    56,    57,
      58,    49,    50,    51,    52,    53,    54,    65,    66,    67,
      68,    69,    70,    -1,    -1,    -1,    74,    55,    56,    57,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    65,    66,    67,
      68,    69,    70,    -1,    -1,    -1,    74,    55,    56,    57,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    65,    66,    67,
      68,    69,    70
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,    88,    89,    84,     0,     5,     4,    75,    90,
      91,     6,     7,    13,    14,    15,    16,    17,    19,    20,
      22,    24,    27,    28,    31,    32,    33,    76,    77,    82,
      83,    84,    85,    92,    93,    94,    95,    96,    97,   101,
     108,   114,   120,   121,   134,    79,    84,    49,    50,    51,
      52,    53,    54,   100,   100,   100,   100,    84,    79,    84,
     134,   134,    84,    77,    47,    48,    77,    77,    79,    79,
      82,   134,    71,    73,    77,    55,    56,    57,    58,    65,
      66,    67,    68,    69,    70,    79,     9,   122,    84,    84,
      84,    84,    75,    73,    79,    79,    73,    79,   134,    77,
      77,   109,   134,    34,    78,   134,   134,   132,   133,   134,
     134,   134,   134,   134,   134,   134,   134,   134,   134,   134,
      84,    75,    71,    73,    79,    71,    79,    79,    98,   134,
     134,    78,   111,   134,   134,    78,    79,    79,    74,    78,
      80,   123,   134,    82,   134,    29,    30,    76,    99,    74,
      74,   102,   134,    81,    78,   115,    71,   134,    10,    11,
      18,    76,    93,   124,   125,    79,    74,    79,    82,    82,
      79,    90,    78,   134,   110,    75,   134,    93,    93,    84,
      71,    79,    79,    79,   103,   112,    81,    90,   116,    79,
      77,   134,    25,   104,    90,   134,    29,    30,    76,   117,
     126,    79,    77,    26,   107,    78,    82,    81,   100,   129,
     130,   131,   134,    90,   113,    81,   119,    84,    78,    80,
      78,    90,   118,    91,    72,    81,   128,   131,   105,    91,
     100,    90,   127,   106,    90,   104
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 11:

/* Line 1455 of yacc.c  */
#line 159 "parser.y"
    {
          delete (yyvsp[(1) - (2)].expr);
      ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 175 "parser.y"
    {
          if (symbolTable.exists((yyvsp[(3) - (4)].str))) {
              printf("Semantic Error: variable %s already declared\n", (yyvsp[(3) - (4)].str));
          } else {
              symbolTable.insert((yyvsp[(3) - (4)].str), (yyvsp[(2) - (4)].str));
              tacGenerator.emitDeclare((yyvsp[(2) - (4)].str), (yyvsp[(3) - (4)].str));
          }
      ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 184 "parser.y"
    {
          if (symbolTable.exists((yyvsp[(3) - (6)].str))) {
              printf("Semantic Error: variable %s already declared\n", (yyvsp[(3) - (6)].str));
          } else {
              symbolTable.insert((yyvsp[(3) - (6)].str), (yyvsp[(2) - (6)].str));
              tacGenerator.emitDeclare((yyvsp[(2) - (6)].str), (yyvsp[(3) - (6)].str));
              tacGenerator.emitAssign((yyvsp[(3) - (6)].str), (yyvsp[(5) - (6)].expr)->place);
          }
          delete (yyvsp[(5) - (6)].expr);
      ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 195 "parser.y"
    {
          if (symbolTable.exists((yyvsp[(3) - (6)].str))) {
              printf("Semantic Error: variable %s already declared\n", (yyvsp[(3) - (6)].str));
          } else {
              symbolTable.insert((yyvsp[(3) - (6)].str), (yyvsp[(2) - (6)].str));
              tacGenerator.emitDeclare((yyvsp[(2) - (6)].str), (yyvsp[(3) - (6)].str));
              tacGenerator.emitAssign((yyvsp[(3) - (6)].str), (yyvsp[(5) - (6)].expr)->place);
          }
          delete (yyvsp[(5) - (6)].expr);
      ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 209 "parser.y"
    {
          if (symbolTable.exists((yyvsp[(3) - (7)].str))) {
              printf("Semantic Error: variable %s already declared\n", (yyvsp[(3) - (7)].str));
          } else {
              symbolTable.insert((yyvsp[(3) - (7)].str), std::string((yyvsp[(2) - (7)].str)) + "[]");
              tacGenerator.emitArrayDecl((yyvsp[(2) - (7)].str), (yyvsp[(3) - (7)].str), toText(static_cast<double>((yyvsp[(5) - (7)].intval))));
          }
      ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 218 "parser.y"
    {
          if (symbolTable.exists((yyvsp[(3) - (9)].str))) {
              printf("Semantic Error: variable %s already declared\n", (yyvsp[(3) - (9)].str));
          } else {
              symbolTable.insert((yyvsp[(3) - (9)].str), std::string((yyvsp[(2) - (9)].str)) + "[]");
              tacGenerator.emitArrayDecl((yyvsp[(2) - (9)].str), (yyvsp[(3) - (9)].str), toText(static_cast<double>((yyvsp[(5) - (9)].intval))));
              tacGenerator.emitArrayStore((yyvsp[(3) - (9)].str), "0", (yyvsp[(8) - (9)].expr)->place);
          }
          delete (yyvsp[(8) - (9)].expr);
      ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 232 "parser.y"
    {
          if (!isDeclaredName((yyvsp[(1) - (4)].str))) {
              printf("Semantic Error: variable %s not declared\n", (yyvsp[(1) - (4)].str));
          } else {
              tacGenerator.emitAssign((yyvsp[(1) - (4)].str), (yyvsp[(3) - (4)].expr)->place);
          }
          delete (yyvsp[(3) - (4)].expr);
      ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 241 "parser.y"
    {
          if (!isDeclaredName((yyvsp[(1) - (7)].str))) {
              printf("Semantic Error: array %s not declared\n", (yyvsp[(1) - (7)].str));
          } else {
              tacGenerator.emitArrayStore((yyvsp[(1) - (7)].str), (yyvsp[(3) - (7)].expr)->place, (yyvsp[(6) - (7)].expr)->place);
          }
          delete (yyvsp[(3) - (7)].expr);
          delete (yyvsp[(6) - (7)].expr);
      ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 254 "parser.y"
    {
          tacGenerator.emitPrint((yyvsp[(2) - (3)].expr)->place);
          delete (yyvsp[(2) - (3)].expr);
      ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 262 "parser.y"
    {
          if (!isDeclaredName((yyvsp[(2) - (3)].str))) {
              printf("Semantic Error: variable %s not declared\n", (yyvsp[(2) - (3)].str));
          } else {
              tacGenerator.emitInput((yyvsp[(2) - (3)].str));
          }
      ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 270 "parser.y"
    {
          if (!isDeclaredName((yyvsp[(2) - (6)].str))) {
              printf("Semantic Error: array %s not declared\n", (yyvsp[(2) - (6)].str));
          } else {
              std::string temp = tacGenerator.newTemp();
              tacGenerator.emitInput(temp);
              tacGenerator.emitArrayStore((yyvsp[(2) - (6)].str), (yyvsp[(4) - (6)].expr)->place, temp);
          }
          delete (yyvsp[(4) - (6)].expr);
      ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 284 "parser.y"
    {
          tacGenerator.emitFunctionReturn();
      ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 288 "parser.y"
    {
          tacGenerator.emitFunctionReturn((yyvsp[(2) - (3)].expr)->place);
          delete (yyvsp[(2) - (3)].expr);
      ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 305 "parser.y"
    { (yyval.str) = strdup("COUNT"); ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 306 "parser.y"
    { (yyval.str) = strdup("REAL"); ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 307 "parser.y"
    { (yyval.str) = strdup("PRECISE"); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 308 "parser.y"
    { (yyval.str) = strdup("FLAG"); ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 309 "parser.y"
    { (yyval.str) = strdup("SYMBOL"); ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 310 "parser.y"
    { (yyval.str) = strdup("VOIDSPACE"); ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 315 "parser.y"
    {
          IfContext ctx{tacGenerator.newLabel(), tacGenerator.newLabel()};
          tacGenerator.emitIfFalseGoto((yyvsp[(3) - (4)].expr)->place, ctx.elseLabel);
          ifStack.push_back(ctx);
          delete (yyvsp[(3) - (4)].expr);
      ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 322 "parser.y"
    {
          IfContext& ctx = ifStack.back();
          tacGenerator.emitGoto(ctx.endLabel);
          tacGenerator.emitLabel(ctx.elseLabel);
      ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 328 "parser.y"
    {
          IfContext ctx = ifStack.back();
          ifStack.pop_back();
          tacGenerator.emitLabel(ctx.endLabel);
      ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 337 "parser.y"
    {
          IfContext& ctx = ifStack.back();
          std::string nextLabel = tacGenerator.newLabel();
          tacGenerator.emitIfFalseGoto((yyvsp[(3) - (4)].expr)->place, nextLabel);
          ctx.elseLabel = nextLabel;
          delete (yyvsp[(3) - (4)].expr);
      ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 345 "parser.y"
    {
          IfContext& ctx = ifStack.back();
          tacGenerator.emitGoto(ctx.endLabel);
          tacGenerator.emitLabel(ctx.elseLabel);
      ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 361 "parser.y"
    {
          LoopContext ctx{tacGenerator.newLabel(), tacGenerator.newLabel(), "", false};
          loopStack.push_back(ctx);
          tacGenerator.emitLabel(ctx.startLabel);
      ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 367 "parser.y"
    {
          LoopContext& ctx = loopStack.back();
          tacGenerator.emitIfFalseGoto((yyvsp[(4) - (5)].expr)->place, ctx.endLabel);
          delete (yyvsp[(4) - (5)].expr);
      ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 373 "parser.y"
    {
          LoopContext ctx = loopStack.back();
          loopStack.pop_back();
          tacGenerator.emitGoto(ctx.startLabel);
          tacGenerator.emitLabel(ctx.endLabel);
      ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 380 "parser.y"
    {
          LoopContext ctx{tacGenerator.newLabel(), tacGenerator.newLabel(), "", false};
          loopStack.push_back(ctx);
          tacGenerator.emitLabel(ctx.startLabel);
      ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 386 "parser.y"
    {
          LoopContext& ctx = loopStack.back();
          tacGenerator.emitIfFalseGoto((yyvsp[(5) - (6)].expr)->place, ctx.endLabel);
          delete (yyvsp[(5) - (6)].expr);
      ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 392 "parser.y"
    {
          LoopContext ctx = loopStack.back();
          loopStack.pop_back();
          tacGenerator.emitGoto(ctx.startLabel);
          tacGenerator.emitLabel(ctx.endLabel);
      ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 399 "parser.y"
    {
          LoopContext ctx{tacGenerator.newLabel(), tacGenerator.newLabel(), (yyvsp[(4) - (9)].expr)->place, true};
          loopStack.push_back(ctx);
          tacGenerator.emitAssign((yyvsp[(4) - (9)].expr)->place, (yyvsp[(6) - (9)].expr)->place);
          tacGenerator.emitLabel(ctx.startLabel);
          std::string conditionTemp = tacGenerator.emitBinary("<", (yyvsp[(4) - (9)].expr)->place, (yyvsp[(8) - (9)].expr)->place);
          tacGenerator.emitIfFalseGoto(conditionTemp, ctx.endLabel);
          delete (yyvsp[(4) - (9)].expr);
          delete (yyvsp[(6) - (9)].expr);
          delete (yyvsp[(8) - (9)].expr);
      ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 411 "parser.y"
    {
          LoopContext ctx = loopStack.back();
          loopStack.pop_back();
          if (ctx.hasIterator) {
              std::string incrementTemp = tacGenerator.emitBinary("+", ctx.iteratorPlace, "1");
              tacGenerator.emitAssign(ctx.iteratorPlace, incrementTemp);
          }
          tacGenerator.emitGoto(ctx.startLabel);
          tacGenerator.emitLabel(ctx.endLabel);
      ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 425 "parser.y"
    {
          SwitchContext ctx{(yyvsp[(3) - (4)].expr)->place, tacGenerator.newLabel(), "", false};
          switchStack.push_back(ctx);
          delete (yyvsp[(3) - (4)].expr);
      ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 431 "parser.y"
    {
          SwitchContext ctx = switchStack.back();
          switchStack.pop_back();
          if (!ctx.nextLabel.empty()) {
              tacGenerator.emitLabel(ctx.nextLabel);
          }
          tacGenerator.emitLabel(ctx.endLabel);
      ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 448 "parser.y"
    {
          SwitchContext& ctx = switchStack.back();
          if (!ctx.nextLabel.empty()) {
              tacGenerator.emitLabel(ctx.nextLabel);
          }
          std::string compareTemp = tacGenerator.emitBinary("==", ctx.exprPlace, toText(static_cast<double>((yyvsp[(2) - (3)].intval))));
          ctx.nextLabel = tacGenerator.newLabel();
          tacGenerator.emitIfFalseGoto(compareTemp, ctx.nextLabel);
      ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 458 "parser.y"
    {
          SwitchContext& ctx = switchStack.back();
          tacGenerator.emitGoto(ctx.endLabel);
      ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 463 "parser.y"
    {
          SwitchContext& ctx = switchStack.back();
          if (!ctx.nextLabel.empty()) {
              tacGenerator.emitLabel(ctx.nextLabel);
              ctx.nextLabel.clear();
          }
          ctx.hasDefault = true;
      ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 476 "parser.y"
    {
          tacGenerator.emit("wait", toText(static_cast<double>((yyvsp[(2) - (4)].intval))), "", "");
      ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 504 "parser.y"
    {
          currentFunctionName = (yyvsp[(2) - (3)].str);
          currentFunctionParams.clear();
      ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 509 "parser.y"
    {
          functionArity[currentFunctionName] = (yyvsp[(5) - (8)].intval);
          tacGenerator.emitFunctionBegin(currentFunctionName);
          for (const std::string& parameterName : currentFunctionParams) {
              tacGenerator.emitParamDef(parameterName);
          }
      ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 517 "parser.y"
    {
          tacGenerator.emitFunctionReturn();
          tacGenerator.emitFunctionEnd(currentFunctionName);
          currentFunctionParams.clear();
      ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 530 "parser.y"
    { (yyval.intval) = (yyvsp[(1) - (1)].intval); ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 531 "parser.y"
    { (yyval.intval) = 0; ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 536 "parser.y"
    {
          (yyval.intval) = (yyvsp[(1) - (3)].intval) + 1;
      ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 540 "parser.y"
    {
          (yyval.intval) = 1;
      ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 547 "parser.y"
    {
          currentFunctionParams.push_back((yyvsp[(2) - (2)].str));
      ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 553 "parser.y"
    { (yyval.intval) = (yyvsp[(1) - (1)].intval); ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 554 "parser.y"
    { (yyval.intval) = 0; ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 559 "parser.y"
    {
          tacGenerator.emitParam((yyvsp[(1) - (1)].expr)->place);
          delete (yyvsp[(1) - (1)].expr);
          (yyval.intval) = 1;
      ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 565 "parser.y"
    {
          tacGenerator.emitParam((yyvsp[(3) - (3)].expr)->place);
          delete (yyvsp[(3) - (3)].expr);
          (yyval.intval) = (yyvsp[(1) - (3)].intval) + 1;
      ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 574 "parser.y"
    {
                std::string place = tacGenerator.emitBinary("<", (yyvsp[(1) - (3)].expr)->place, (yyvsp[(3) - (3)].expr)->place);
                (yyval.expr) = makeExpr(place, ((yyvsp[(1) - (3)].expr)->value < (yyvsp[(3) - (3)].expr)->value) ? 1.0 : 0.0, (yyvsp[(1) - (3)].expr)->isConst && (yyvsp[(3) - (3)].expr)->isConst);
                delete (yyvsp[(1) - (3)].expr);
                delete (yyvsp[(3) - (3)].expr);
            ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 581 "parser.y"
    {
                std::string place = tacGenerator.emitBinary(">", (yyvsp[(1) - (3)].expr)->place, (yyvsp[(3) - (3)].expr)->place);
                (yyval.expr) = makeExpr(place, ((yyvsp[(1) - (3)].expr)->value > (yyvsp[(3) - (3)].expr)->value) ? 1.0 : 0.0, (yyvsp[(1) - (3)].expr)->isConst && (yyvsp[(3) - (3)].expr)->isConst);
                delete (yyvsp[(1) - (3)].expr);
                delete (yyvsp[(3) - (3)].expr);
            ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 588 "parser.y"
    {
                std::string place = tacGenerator.emitBinary("<=", (yyvsp[(1) - (3)].expr)->place, (yyvsp[(3) - (3)].expr)->place);
                (yyval.expr) = makeExpr(place, ((yyvsp[(1) - (3)].expr)->value <= (yyvsp[(3) - (3)].expr)->value) ? 1.0 : 0.0, (yyvsp[(1) - (3)].expr)->isConst && (yyvsp[(3) - (3)].expr)->isConst);
                delete (yyvsp[(1) - (3)].expr);
                delete (yyvsp[(3) - (3)].expr);
            ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 595 "parser.y"
    {
                std::string place = tacGenerator.emitBinary(">=", (yyvsp[(1) - (3)].expr)->place, (yyvsp[(3) - (3)].expr)->place);
                (yyval.expr) = makeExpr(place, ((yyvsp[(1) - (3)].expr)->value >= (yyvsp[(3) - (3)].expr)->value) ? 1.0 : 0.0, (yyvsp[(1) - (3)].expr)->isConst && (yyvsp[(3) - (3)].expr)->isConst);
                delete (yyvsp[(1) - (3)].expr);
                delete (yyvsp[(3) - (3)].expr);
            ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 602 "parser.y"
    {
                std::string place = tacGenerator.emitBinary("==", (yyvsp[(1) - (3)].expr)->place, (yyvsp[(3) - (3)].expr)->place);
                (yyval.expr) = makeExpr(place, ((yyvsp[(1) - (3)].expr)->value == (yyvsp[(3) - (3)].expr)->value) ? 1.0 : 0.0, (yyvsp[(1) - (3)].expr)->isConst && (yyvsp[(3) - (3)].expr)->isConst);
                delete (yyvsp[(1) - (3)].expr);
                delete (yyvsp[(3) - (3)].expr);
            ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 609 "parser.y"
    {
                std::string place = tacGenerator.emitBinary("!=", (yyvsp[(1) - (3)].expr)->place, (yyvsp[(3) - (3)].expr)->place);
                (yyval.expr) = makeExpr(place, ((yyvsp[(1) - (3)].expr)->value != (yyvsp[(3) - (3)].expr)->value) ? 1.0 : 0.0, (yyvsp[(1) - (3)].expr)->isConst && (yyvsp[(3) - (3)].expr)->isConst);
                delete (yyvsp[(1) - (3)].expr);
                delete (yyvsp[(3) - (3)].expr);
            ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 616 "parser.y"
    {
          std::string place = tacGenerator.emitBinary("+", (yyvsp[(1) - (3)].expr)->place, (yyvsp[(3) - (3)].expr)->place);
                    (yyval.expr) = makeExpr(place, 0.0, false);
          delete (yyvsp[(1) - (3)].expr);
          delete (yyvsp[(3) - (3)].expr);
      ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 623 "parser.y"
    {
          std::string place = tacGenerator.emitBinary("-", (yyvsp[(1) - (3)].expr)->place, (yyvsp[(3) - (3)].expr)->place);
                    (yyval.expr) = makeExpr(place, 0.0, false);
          delete (yyvsp[(1) - (3)].expr);
          delete (yyvsp[(3) - (3)].expr);
      ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 630 "parser.y"
    {
          std::string place = tacGenerator.emitBinary("*", (yyvsp[(1) - (3)].expr)->place, (yyvsp[(3) - (3)].expr)->place);
                    (yyval.expr) = makeExpr(place, 0.0, false);
          delete (yyvsp[(1) - (3)].expr);
          delete (yyvsp[(3) - (3)].expr);
      ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 637 "parser.y"
    {
          std::string place = tacGenerator.emitBinary("/", (yyvsp[(1) - (3)].expr)->place, (yyvsp[(3) - (3)].expr)->place);
                    (yyval.expr) = makeExpr(place, 0.0, false);
          delete (yyvsp[(1) - (3)].expr);
          delete (yyvsp[(3) - (3)].expr);
      ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 644 "parser.y"
    {
          (yyval.expr) = makeNumericExpr(static_cast<double>((yyvsp[(1) - (1)].intval)));
      ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 648 "parser.y"
    {
          (yyval.expr) = makeNumericExpr((yyvsp[(1) - (1)].floatval));
      ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 652 "parser.y"
    {
                    (yyval.expr) = makeExpr((yyvsp[(1) - (1)].str), 0.0, true);
            ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 656 "parser.y"
    {
          if (!isDeclaredName((yyvsp[(1) - (1)].str))) {
              printf("Semantic Error: variable %s not declared\n", (yyvsp[(1) - (1)].str));
              (yyval.expr) = makeExpr((yyvsp[(1) - (1)].str), 0.0, false);
          } else {
                            (yyval.expr) = makeExpr((yyvsp[(1) - (1)].str), 0.0, false, true);
          }
      ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 665 "parser.y"
    {
                      if (!isDeclaredName((yyvsp[(1) - (4)].str))) {
                            printf("Semantic Error: array %s not declared\n", (yyvsp[(1) - (4)].str));
                            (yyval.expr) = makeExpr("0", 0.0, false);
                    } else {
                            std::string temp = tacGenerator.emitArrayLoad((yyvsp[(1) - (4)].str), (yyvsp[(3) - (4)].expr)->place);
                            (yyval.expr) = makeExpr(temp, 0.0, false);
                    }
                    delete (yyvsp[(3) - (4)].expr);
            ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 676 "parser.y"
    {
                    std::string callTemp = tacGenerator.emitCall((yyvsp[(1) - (4)].str), (yyvsp[(3) - (4)].intval));
                    (yyval.expr) = makeExpr(callTemp, 0.0, false);
            ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 681 "parser.y"
    {
          (yyval.expr) = (yyvsp[(2) - (3)].expr);
      ;}
    break;



/* Line 1455 of yacc.c  */
#line 2537 "parser.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 687 "parser.y"


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

