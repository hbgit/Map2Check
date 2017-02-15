/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         smtparse
#define yylex           smtlex
#define yyerror         smterror
#define yydebug         smtdebug
#define yynerrs         smtnerrs

#define yylval          smtlval
#define yychar          smtchar

/* Copy the first part of user declarations.  */
#line 1 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:339  */

  /********************************************************************
   * AUTHORS: Vijay Ganesh, Trevor Hansen
   *
   * BEGIN DATE: July, 2006
   *
   * This file is modified version of the CVCL's smtlib.y file. Please
   * see CVCL license below
  ********************************************************************/
  
  /********************************************************************
   *
   * \file smtlib.y
   * 
   * Author: Sergey Berezin, Clark Barrett
   * 
   * Created: Apr 30 2005
   *
   * <hr>
   * Copyright (C) 2004 by the Board of Trustees of Leland Stanford
   * Junior University and by New York University. 
   *
   * License to use, copy, modify, sell and/or distribute this software
   * and its documentation for any purpose is hereby granted without
   * royalty, subject to the terms and conditions defined in the \ref
   * LICENSE file provided with this distribution.  In particular:
   *
   * - The above copyright notice and this permission notice must appear
   * in all copies of the software and related documentation.
   *
   * - THE SOFTWARE IS PROVIDED "AS-IS", WITHOUT ANY WARRANTIES,
   * EXPRESSED OR IMPLIED.  USE IT AT YOUR OWN RISK.
   * 
   * <hr>
  ********************************************************************/

#include "stp/cpp_interface.h"
#include "stp/Parser/LetMgr.h"

  using namespace stp;
  using std::cout;
  using std::cerr;
  using std::endl;

  // Suppress the bogus warning suppression in bison (it generates
  // compile error)
#undef __GNUC_MINOR__
  
  extern char* smttext;
  extern int smtlineno;
  extern int smtlex(void);

  int yyerror(const char *s) {
    cout << "syntax error: line " << smtlineno << "\n" << s << endl;
    cout << "  token: " << smttext << endl;
    FatalError("");
    return 1;
  }
  int yyerror(void* /*AssertsQuery*/, const char* s) { return yyerror(s); }

  ASTNode query;
#define YYLTYPE_IS_TRIVIAL 1
#define YYMAXDEPTH 104857600
#define YYERROR_VERBOSE 1
#define YY_EXIT_FAILURE -1
  

#line 142 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "parsesmt.hpp".  */
#ifndef YY_SMT_HOME_MAP2CHECK_DEVEL_TOOL_MAP_SRC_ON_DOCKER_DEPENDENCIES_STP_LIB_PARSER_PARSESMT_HPP_INCLUDED
# define YY_SMT_HOME_MAP2CHECK_DEVEL_TOOL_MAP_SRC_ON_DOCKER_DEPENDENCIES_STP_LIB_PARSER_PARSESMT_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int smtdebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    NUMERAL_TOK = 258,
    BVCONST_TOK = 259,
    BITCONST_TOK = 260,
    FORMID_TOK = 261,
    TERMID_TOK = 262,
    STRING_TOK = 263,
    USER_VAL_TOK = 264,
    SOURCE_TOK = 265,
    CATEGORY_TOK = 266,
    DIFFICULTY_TOK = 267,
    BITVEC_TOK = 268,
    ARRAY_TOK = 269,
    SELECT_TOK = 270,
    STORE_TOK = 271,
    TRUE_TOK = 272,
    FALSE_TOK = 273,
    NOT_TOK = 274,
    IMPLIES_TOK = 275,
    ITE_TOK = 276,
    AND_TOK = 277,
    OR_TOK = 278,
    XOR_TOK = 279,
    IFF_TOK = 280,
    EXISTS_TOK = 281,
    FORALL_TOK = 282,
    LET_TOK = 283,
    FLET_TOK = 284,
    NOTES_TOK = 285,
    CVC_COMMAND_TOK = 286,
    SORTS_TOK = 287,
    FUNS_TOK = 288,
    PREDS_TOK = 289,
    EXTENSIONS_TOK = 290,
    DEFINITION_TOK = 291,
    AXIOMS_TOK = 292,
    LOGIC_TOK = 293,
    COLON_TOK = 294,
    LBRACKET_TOK = 295,
    RBRACKET_TOK = 296,
    LPAREN_TOK = 297,
    RPAREN_TOK = 298,
    SAT_TOK = 299,
    UNSAT_TOK = 300,
    UNKNOWN_TOK = 301,
    ASSUMPTION_TOK = 302,
    FORMULA_TOK = 303,
    STATUS_TOK = 304,
    BENCHMARK_TOK = 305,
    EXTRASORTS_TOK = 306,
    EXTRAFUNS_TOK = 307,
    EXTRAPREDS_TOK = 308,
    LANGUAGE_TOK = 309,
    DOLLAR_TOK = 310,
    QUESTION_TOK = 311,
    DISTINCT_TOK = 312,
    SEMICOLON_TOK = 313,
    EOF_TOK = 314,
    EQ_TOK = 315,
    NAND_TOK = 316,
    NOR_TOK = 317,
    NEQ_TOK = 318,
    ASSIGN_TOK = 319,
    BV_TOK = 320,
    BOOLEAN_TOK = 321,
    BVLEFTSHIFT_1_TOK = 322,
    BVRIGHTSHIFT_1_TOK = 323,
    BVARITHRIGHTSHIFT_TOK = 324,
    BVPLUS_TOK = 325,
    BVSUB_TOK = 326,
    BVNOT_TOK = 327,
    BVMULT_TOK = 328,
    BVDIV_TOK = 329,
    SBVDIV_TOK = 330,
    BVMOD_TOK = 331,
    SBVREM_TOK = 332,
    SBVMOD_TOK = 333,
    BVNEG_TOK = 334,
    BVAND_TOK = 335,
    BVOR_TOK = 336,
    BVXOR_TOK = 337,
    BVNAND_TOK = 338,
    BVNOR_TOK = 339,
    BVXNOR_TOK = 340,
    BVCONCAT_TOK = 341,
    BVLT_TOK = 342,
    BVGT_TOK = 343,
    BVLE_TOK = 344,
    BVGE_TOK = 345,
    BVSLT_TOK = 346,
    BVSGT_TOK = 347,
    BVSLE_TOK = 348,
    BVSGE_TOK = 349,
    BVSX_TOK = 350,
    BVZX_TOK = 351,
    BVROTATE_RIGHT_TOK = 352,
    BVROTATE_LEFT_TOK = 353,
    BVREPEAT_TOK = 354,
    BVCOMP_TOK = 355,
    BOOLEXTRACT_TOK = 356,
    BOOL_TO_BV_TOK = 357,
    BVEXTRACT_TOK = 358
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 70 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:355  */
  
  // FIXME: Why is this not an UNSIGNED int?
  int uintval;                  /* for numerals in types. */

  // for BV32 BVCONST 
  unsigned long long ullval;

  struct {
    //stores the indexwidth and valuewidth
    //indexwidth is 0 iff type is bitvector. positive iff type is
    //array, and stores the width of the indexing bitvector
    unsigned int indexwidth;
    //width of the bitvector type
    unsigned int valuewidth;
  } indexvaluewidth;

  //ASTNode,ASTVec
  stp::ASTNode *node;
  stp::ASTVec *vec;
  std::string *str;

#line 308 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE smtlval;

int smtparse (void* AssertsQuery);

#endif /* !YY_SMT_HOME_MAP2CHECK_DEVEL_TOOL_MAP_SRC_ON_DOCKER_DEPENDENCIES_STP_LIB_PARSER_PARSESMT_HPP_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 325 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:358  */

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
#else
typedef signed char yytype_int8;
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
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   431

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  104
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  23
/* YYNRULES -- Number of rules.  */
#define YYNRULES  101
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  271

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   358

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
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
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   212,   212,   238,   260,   266,   275,   287,   292,   303,
     307,   318,   322,   326,   333,   337,   344,   348,   352,   370,
     373,   379,   387,   390,   393,   397,   403,   414,   417,   424,
     434,   447,   456,   467,   473,   479,   483,   490,   517,   525,
     533,   541,   549,   557,   565,   573,   581,   585,   590,   596,
     603,   608,   613,   619,   625,   634,   649,   658,   667,   678,
     683,   688,   695,   696,   701,   705,   718,   732,   748,   756,
     764,   772,   780,   788,   796,   804,   823,   838,   846,   854,
     862,   871,   880,   889,   897,   905,   913,   921,   930,   939,
     948,   979,  1010,  1026,  1034,  1045,  1060,  1081,  1086,  1091,
    1098,  1102
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NUMERAL_TOK", "BVCONST_TOK",
  "BITCONST_TOK", "FORMID_TOK", "TERMID_TOK", "STRING_TOK", "USER_VAL_TOK",
  "SOURCE_TOK", "CATEGORY_TOK", "DIFFICULTY_TOK", "BITVEC_TOK",
  "ARRAY_TOK", "SELECT_TOK", "STORE_TOK", "TRUE_TOK", "FALSE_TOK",
  "NOT_TOK", "IMPLIES_TOK", "ITE_TOK", "AND_TOK", "OR_TOK", "XOR_TOK",
  "IFF_TOK", "EXISTS_TOK", "FORALL_TOK", "LET_TOK", "FLET_TOK",
  "NOTES_TOK", "CVC_COMMAND_TOK", "SORTS_TOK", "FUNS_TOK", "PREDS_TOK",
  "EXTENSIONS_TOK", "DEFINITION_TOK", "AXIOMS_TOK", "LOGIC_TOK",
  "COLON_TOK", "LBRACKET_TOK", "RBRACKET_TOK", "LPAREN_TOK", "RPAREN_TOK",
  "SAT_TOK", "UNSAT_TOK", "UNKNOWN_TOK", "ASSUMPTION_TOK", "FORMULA_TOK",
  "STATUS_TOK", "BENCHMARK_TOK", "EXTRASORTS_TOK", "EXTRAFUNS_TOK",
  "EXTRAPREDS_TOK", "LANGUAGE_TOK", "DOLLAR_TOK", "QUESTION_TOK",
  "DISTINCT_TOK", "SEMICOLON_TOK", "EOF_TOK", "EQ_TOK", "NAND_TOK",
  "NOR_TOK", "NEQ_TOK", "ASSIGN_TOK", "BV_TOK", "BOOLEAN_TOK",
  "BVLEFTSHIFT_1_TOK", "BVRIGHTSHIFT_1_TOK", "BVARITHRIGHTSHIFT_TOK",
  "BVPLUS_TOK", "BVSUB_TOK", "BVNOT_TOK", "BVMULT_TOK", "BVDIV_TOK",
  "SBVDIV_TOK", "BVMOD_TOK", "SBVREM_TOK", "SBVMOD_TOK", "BVNEG_TOK",
  "BVAND_TOK", "BVOR_TOK", "BVXOR_TOK", "BVNAND_TOK", "BVNOR_TOK",
  "BVXNOR_TOK", "BVCONCAT_TOK", "BVLT_TOK", "BVGT_TOK", "BVLE_TOK",
  "BVGE_TOK", "BVSLT_TOK", "BVSGT_TOK", "BVSLE_TOK", "BVSGE_TOK",
  "BVSX_TOK", "BVZX_TOK", "BVROTATE_RIGHT_TOK", "BVROTATE_LEFT_TOK",
  "BVREPEAT_TOK", "BVCOMP_TOK", "BOOLEXTRACT_TOK", "BOOL_TO_BV_TOK",
  "BVEXTRACT_TOK", "$accept", "cmd", "benchmark", "bench_name",
  "bench_attributes", "bench_attribute", "logic_name", "status",
  "annotation", "user_value", "attribute", "sort_symbs", "var_decls",
  "var_decl", "an_formulas", "an_formula", "letexpr_mgmt", "an_terms",
  "an_term", "an_nonbvconst_term", "sort_symb", "var", "fvar", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
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
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358
};
# endif

#define YYPACT_NINF -81

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-81)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -16,   -23,    28,   -81,    21,   -81,   -81,    -9,   248,   -28,
     -81,   -81,    24,   -81,   -81,   -81,    27,   197,   197,   -27,
       0,    14,   -81,   -81,   -81,   -81,    17,   -81,   -81,   -81,
     -81,   337,    52,   -81,   197,   -81,   -81,   -81,   -81,   -81,
     -81,    18,    18,    56,   197,   197,   197,   197,   197,   197,
     197,    20,    22,   250,   250,   250,   250,   250,   250,   250,
     250,   250,   250,    23,   -81,    33,    57,   -22,   -81,   -20,
      36,    35,   197,   197,    80,   -81,   127,   197,   197,    25,
      29,    39,   -81,   -81,   -81,   250,   250,   197,   250,    73,
     250,   250,   250,   250,   250,   250,   250,   250,   250,   250,
     250,   250,   250,   250,   250,   250,   250,   250,   250,   250,
      42,    43,    45,    47,    48,   250,    49,   150,   -81,   -81,
     -81,   250,   250,   250,   250,   250,   250,   250,   250,   250,
     -81,   -81,    -4,   -81,   -81,   -81,   -81,   -81,    50,   197,
     -81,   -81,   -81,    61,    68,    82,    83,    89,   250,   250,
     250,    76,   -81,   250,   250,   250,   250,   250,   -81,   250,
     250,   250,   250,   250,   250,   -81,   250,   250,   250,   250,
     250,   250,   250,   122,   123,   124,   125,   126,   250,   128,
     -81,   -81,    87,    93,    95,    96,    97,    98,    99,   100,
     115,    92,   119,   -81,   117,    11,   -81,   118,   -81,   -81,
     250,   197,   121,   -81,   250,   250,   -81,   -81,   -81,   -81,
     -81,   -81,   -81,   -81,   -81,   -81,   -81,   -81,   -81,   -81,
     -81,   -81,   -81,   131,   132,   133,   137,   142,   -81,   129,
     -81,   -81,   -81,   -81,   -81,   -81,   -81,   -81,   -81,   160,
     161,   -81,   -81,   -81,   141,   143,   -81,   -81,   -81,   250,
     250,   250,   250,   250,   164,   144,   148,   -81,   -81,   -81,
     -81,   -81,   -81,   -81,   147,   -81,   186,   250,   153,   -81,
     -81
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     2,     0,     1,     4,     0,     0,     0,
       5,    13,    19,    22,    23,    24,     0,     0,     0,     0,
       0,     0,     3,     6,    21,    20,    15,    10,   101,    33,
      34,     0,     0,     7,     0,    35,     8,    16,    17,    18,
       9,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   100,     0,     0,     0,    27,     0,
       0,     0,     0,     0,     0,    31,     0,     0,     0,     0,
       0,    59,    62,    97,    98,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    57,    61,
      63,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      46,    54,     0,    11,    28,    12,    14,    47,     0,     0,
      50,    32,    51,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    99,     0,     0,     0,    78,     0,    70,    79,
       0,     0,     0,     0,     0,    71,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      37,    58,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    30,     0,    25,    48,     0,    52,    53,
       0,     0,     0,    65,     0,     0,    64,    87,    88,    89,
      77,    80,    82,    81,    83,    84,    72,    73,    74,    85,
      86,    75,    69,     0,     0,     0,     0,     0,    76,     0,
      36,    42,    44,    43,    45,    38,    40,    39,    41,     0,
       0,    29,    26,    49,     0,     0,    60,    66,    68,     0,
       0,     0,     0,     0,     0,     0,     0,    55,    56,    93,
      94,    91,    90,    92,     0,    95,     0,     0,     0,    67,
      96
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -81,   -81,   -81,   -81,   -81,   182,   -81,   -81,   -81,   -81,
     -81,   -81,   158,   -55,   154,   103,   -81,   -80,   -54,   -81,
       6,   -81,   -81
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,     7,     9,    10,    27,    40,    11,    25,
      12,   194,    67,    68,    74,    75,    34,   117,   118,   119,
     195,   120,    35
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint16 yytable[] =
{
     121,   122,   123,   124,   125,   126,   127,   128,   129,   191,
     192,     8,   134,   156,   134,    22,   159,    37,    38,    39,
      66,   133,    66,   135,   191,   192,     1,     4,     5,     6,
       8,   148,   149,    24,   151,    26,   153,   154,   155,   193,
     157,   158,    41,   160,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,    42,    43,    64,    70,
      66,   178,    79,   181,    80,   132,   130,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   131,   136,   137,   147,
     152,   145,   173,   174,   146,   175,    28,   176,   177,   179,
     200,   201,   202,   196,   203,   204,   205,    29,    30,   207,
     208,   209,   181,   210,   198,   181,   211,   212,   213,   214,
     215,   199,   216,   217,   218,   219,   220,   221,   222,   206,
      33,    36,    31,   140,   228,   223,   224,   225,   226,   227,
     230,   229,   239,    28,    63,    32,   231,    65,   232,   233,
     234,   235,   236,   237,    29,    30,   244,    71,    72,    73,
     247,   248,    77,    78,    81,    82,    83,    84,   238,   240,
     241,   243,   246,   255,   256,    85,    86,   264,   254,    31,
     142,    87,   249,   250,   251,   138,   139,   141,   252,   141,
     143,   144,    32,   253,   257,   265,   258,   266,   267,   268,
     150,    23,    88,   180,   270,   259,   260,   261,   262,   263,
      69,   242,    76,    28,     0,     0,    89,     0,     0,     0,
       0,     0,     0,   269,    29,    30,     0,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,     0,     0,    31,
       0,     0,   197,     0,     0,   110,   111,   112,   113,   114,
     115,     0,    32,   116,    81,    82,    83,    84,    13,    14,
      15,     0,     0,     0,     0,    85,    86,     0,     0,     0,
       0,    87,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    16,     0,     0,     0,
       0,     0,    88,     0,     0,    17,    18,    19,     0,     0,
      20,    21,     0,     0,   245,     0,    89,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,     0,     0,     0,
       0,     0,     0,    28,     0,   110,   111,   112,   113,   114,
     115,     0,     0,   116,    29,    30,    44,    45,    46,    47,
      48,    49,    50,     0,     0,    51,    52,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    31,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    32,     0,    53,     0,     0,    54,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    55,    56,    57,    58,    59,    60,
      61,    62
};

static const yytype_int16 yycheck[] =
{
      54,    55,    56,    57,    58,    59,    60,    61,    62,    13,
      14,    39,    67,    93,    69,    43,    96,    44,    45,    46,
      42,    43,    42,    43,    13,    14,    42,    50,     0,     8,
      39,    85,    86,     9,    88,     8,    90,    91,    92,    43,
      94,    95,    42,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,    42,    40,     6,     3,
      42,   115,    42,   117,    42,     8,    43,   121,   122,   123,
     124,   125,   126,   127,   128,   129,    43,    41,    43,    40,
       7,    56,    40,    40,    55,    40,     6,    40,    40,    40,
       8,     8,     3,    43,   148,   149,   150,    17,    18,   153,
     154,   155,   156,   157,    43,   159,   160,   161,   162,   163,
     164,    43,   166,   167,   168,   169,   170,   171,   172,    43,
      17,    18,    42,    43,   178,     3,     3,     3,     3,     3,
      43,     3,    40,     6,    31,    55,    43,    34,    43,    43,
      43,    43,    43,    43,    17,    18,   200,    44,    45,    46,
     204,   205,    49,    50,     4,     5,     6,     7,    43,    40,
      43,    43,    41,     3,     3,    15,    16,     3,    39,    42,
      43,    21,    41,    41,    41,    72,    73,    74,    41,    76,
      77,    78,    55,    41,    43,    41,    43,    39,    41,     3,
      87,     9,    42,    43,    41,   249,   250,   251,   252,   253,
      42,   195,    48,     6,    -1,    -1,    56,    -1,    -1,    -1,
      -1,    -1,    -1,   267,    17,    18,    -1,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    -1,    -1,    42,
      -1,    -1,   139,    -1,    -1,    95,    96,    97,    98,    99,
     100,    -1,    55,   103,     4,     5,     6,     7,    10,    11,
      12,    -1,    -1,    -1,    -1,    15,    16,    -1,    -1,    -1,
      -1,    21,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    38,    -1,    -1,    -1,
      -1,    -1,    42,    -1,    -1,    47,    48,    49,    -1,    -1,
      52,    53,    -1,    -1,   201,    -1,    56,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    -1,    -1,    -1,
      -1,    -1,    -1,     6,    -1,    95,    96,    97,    98,    99,
     100,    -1,    -1,   103,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    -1,    -1,    28,    29,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    55,    -1,    57,    -1,    -1,    60,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    87,    88,    89,    90,    91,    92,
      93,    94
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    42,   105,   106,    50,     0,     8,   107,    39,   108,
     109,   112,   114,    10,    11,    12,    38,    47,    48,    49,
      52,    53,    43,   109,     9,   113,     8,   110,     6,    17,
      18,    42,    55,   119,   120,   126,   119,    44,    45,    46,
     111,    42,    42,    40,    19,    20,    21,    22,    23,    24,
      25,    28,    29,    57,    60,    87,    88,    89,    90,    91,
      92,    93,    94,   119,     6,   119,    42,   116,   117,   116,
       3,   119,   119,   119,   118,   119,   118,   119,   119,    42,
      42,     4,     5,     6,     7,    15,    16,    21,    42,    56,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      95,    96,    97,    98,    99,   100,   103,   121,   122,   123,
     125,   122,   122,   122,   122,   122,   122,   122,   122,   122,
      43,    43,     8,    43,   117,    43,    41,    43,   119,   119,
      43,   119,    43,   119,   119,    56,    55,    40,   122,   122,
     119,   122,     7,   122,   122,   122,   121,   122,   122,   121,
     122,   122,   122,   122,   122,   122,   122,   122,   122,   122,
     122,   122,   122,    40,    40,    40,    40,    40,   122,    40,
      43,   122,   122,   122,   122,   122,   122,   122,   122,   122,
     122,    13,    14,    43,   115,   124,    43,   119,    43,    43,
       8,     8,     3,   122,   122,   122,    43,   122,   122,   122,
     122,   122,   122,   122,   122,   122,   122,   122,   122,   122,
     122,   122,   122,     3,     3,     3,     3,     3,   122,     3,
      43,    43,    43,    43,    43,    43,    43,    43,    43,    40,
      40,    43,   124,    43,   122,   119,    41,   122,   122,    41,
      41,    41,    41,    41,    39,     3,     3,    43,    43,   122,
     122,   122,   122,   122,     3,    41,    39,    41,     3,   122,
      41
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   104,   105,   106,   107,   108,   108,   109,   109,   109,
     109,   109,   109,   109,   110,   110,   111,   111,   111,   112,
     112,   113,   114,   114,   114,   115,   115,   116,   116,   117,
     117,   118,   118,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   120,   120,   121,   121,   122,
     122,   122,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   124,   124,   125,   125,   125,
     126,   126
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     5,     1,     1,     2,     3,     3,     3,
       3,     5,     5,     1,     4,     1,     1,     1,     1,     1,
       2,     1,     2,     2,     2,     1,     2,     1,     2,     4,
       3,     1,     2,     1,     1,     1,     5,     4,     5,     5,
       5,     5,     5,     5,     5,     5,     3,     4,     5,     6,
       4,     4,     5,     5,     3,     7,     7,     1,     2,     1,
       4,     1,     1,     1,     3,     3,     4,     7,     4,     3,
       2,     2,     3,     3,     3,     3,     3,     3,     2,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       5,     5,     5,     5,     5,     4,     6,     1,     1,     2,
       2,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (AssertsQuery, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, AssertsQuery); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void* AssertsQuery)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (AssertsQuery);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void* AssertsQuery)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, AssertsQuery);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, void* AssertsQuery)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              , AssertsQuery);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, AssertsQuery); \
} while (0)

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
#ifndef YYINITDEPTH
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
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, void* AssertsQuery)
{
  YYUSE (yyvaluep);
  YYUSE (AssertsQuery);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void* AssertsQuery)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
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
  int yytoken = 0;
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

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
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
      if (yytable_value_is_error (yyn))
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
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 213 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  ASTNode assumptions;
  if((yyvsp[0].node) == NULL) 
    {
      assumptions = GlobalParserInterface->CreateNode(TRUE);
    } 
  else 
    {
      assumptions = *(yyvsp[0].node);
    }
      
  if(query.IsNull()) 
    {
      query = GlobalParserInterface->CreateNode(FALSE);
    }

  ((ASTVec*)AssertsQuery)->push_back(assumptions);
  ((ASTVec*)AssertsQuery)->push_back(query);
  delete (yyvsp[0].node);
  query = ASTNode();
  YYACCEPT;
}
#line 1659 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 3:
#line 239 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  if((yyvsp[-1].vec) != NULL){
    if((yyvsp[-1].vec)->size() > 1) 
      (yyval.node) = new ASTNode(GlobalParserInterface->CreateNode(AND,*(yyvsp[-1].vec)));
    else if((yyvsp[-1].vec)->size() ==1)
      (yyval.node) = new ASTNode((*(yyvsp[-1].vec))[0]);
     else
      (yyval.node) = new ASTNode(GlobalParserInterface->CreateNode(TRUE));     
    delete (yyvsp[-1].vec);
  }
  else {
    (yyval.node) = NULL;
  }
  delete (yyvsp[-2].str); //discard the benchmarkname.
}
#line 1679 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 4:
#line 261 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
}
#line 1686 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 5:
#line 267 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.vec) = new ASTVec;
  if ((yyvsp[0].node) != NULL) {
    (yyval.vec)->push_back(*(yyvsp[0].node));
    GlobalParserInterface->AddAssert(*(yyvsp[0].node));
    delete (yyvsp[0].node);
  }
}
#line 1699 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 6:
#line 276 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  if ((yyvsp[-1].vec) != NULL && (yyvsp[0].node) != NULL) {
    (yyvsp[-1].vec)->push_back(*(yyvsp[0].node));
    GlobalParserInterface->AddAssert(*(yyvsp[0].node));
    (yyval.vec) = (yyvsp[-1].vec);
    delete (yyvsp[0].node);
  }
}
#line 1712 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 7:
#line 288 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  //assumptions are like asserts
  (yyval.node) = (yyvsp[0].node);
}
#line 1721 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 8:
#line 293 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  // Previously this would call AddQuery() on the negation.
  // But if multiple formula were (eroneously) present
  // it discarded all but the last formula. Allowing multiple 
  // formula and taking the conjunction of them along with all
  // the assumptions is what the other solvers do.  

  //assumptions are like asserts
  (yyval.node) = (yyvsp[0].node);
}
#line 1736 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 9:
#line 304 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = NULL;
}
#line 1744 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 10:
#line 308 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  if (!(0 == strcmp((yyvsp[0].str)->c_str(),"QF_UFBV")  ||
        0 == strcmp((yyvsp[0].str)->c_str(),"QF_BV") ||
        //0 == strcmp($3->c_str(),"QF_UF") ||
        0 == strcmp((yyvsp[0].str)->c_str(),"QF_AUFBV"))) {
    yyerror("Wrong input logic:");
  }
  delete (yyvsp[0].str);
  (yyval.node) = NULL;
}
#line 1759 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 11:
#line 319 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = NULL;
}
#line 1767 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 12:
#line 323 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = NULL;
}
#line 1775 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 13:
#line 327 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = NULL;
}
#line 1783 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 14:
#line 334 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.str) = (yyvsp[-3].str);
}
#line 1791 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 15:
#line 338 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.str) = (yyvsp[0].str);
}
#line 1799 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 16:
#line 344 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    { 
  input_status = TO_BE_SATISFIABLE; 
  (yyval.node) = NULL; 
}
#line 1808 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 17:
#line 348 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    { 
  input_status = TO_BE_UNSATISFIABLE; 
  (yyval.node) = NULL; 
  }
#line 1817 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 18:
#line 353 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    { 
  input_status = TO_BE_UNKNOWN; 
  (yyval.node) = NULL; 
}
#line 1826 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 19:
#line 371 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
}
#line 1833 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 20:
#line 374 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
}
#line 1840 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 21:
#line 380 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  //cerr << "Printing user_value: " << *$1 << endl;
  delete (yyvsp[0].str);
}
#line 1849 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 22:
#line 388 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
}
#line 1856 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 23:
#line 391 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
}
#line 1863 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 25:
#line 398 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  //a single sort symbol here means either a BitVec or a Boolean
  (yyval.indexvaluewidth).indexwidth = (yyvsp[0].indexvaluewidth).indexwidth;
  (yyval.indexvaluewidth).valuewidth = (yyvsp[0].indexvaluewidth).valuewidth;
}
#line 1873 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 26:
#line 404 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  //two sort symbols mean f: type --> type
  (yyval.indexvaluewidth).indexwidth = (yyvsp[-1].indexvaluewidth).valuewidth;
  (yyval.indexvaluewidth).valuewidth = (yyvsp[0].indexvaluewidth).valuewidth;
}
#line 1883 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 27:
#line 415 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {}
#line 1889 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 28:
#line 418 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {}
#line 1895 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 29:
#line 425 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  ASTNode s = stp::GlobalParserInterface->LookupOrCreateSymbol((yyvsp[-2].str)->c_str());
  //Sort_symbs has the indexwidth/valuewidth. Set those fields in
  //var
  s.SetIndexWidth((yyvsp[-1].indexvaluewidth).indexwidth);
  s.SetValueWidth((yyvsp[-1].indexvaluewidth).valuewidth);
  GlobalParserInterface->addSymbol(s);
  delete (yyvsp[-2].str);
}
#line 1909 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 30:
#line 435 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  ASTNode s = stp::GlobalParserInterface->LookupOrCreateSymbol((yyvsp[-1].str)->c_str());
  s.SetIndexWidth(0);
  s.SetValueWidth(0);
  GlobalParserInterface->addSymbol(s);
  //Sort_symbs has the indexwidth/valuewidth. Set those fields in
  //var
  delete (yyvsp[-1].str);
}
#line 1923 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 31:
#line 448 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.vec) = new ASTVec;
  if ((yyvsp[0].node) != NULL) {
    (yyval.vec)->push_back(*(yyvsp[0].node));
    delete (yyvsp[0].node);
  }
}
#line 1935 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 32:
#line 457 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  if ((yyvsp[-1].vec) != NULL && (yyvsp[0].node) != NULL) {
    (yyvsp[-1].vec)->push_back(*(yyvsp[0].node));
    (yyval.vec) = (yyvsp[-1].vec);
    delete (yyvsp[0].node);
  }
}
#line 1947 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 33:
#line 468 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode(GlobalParserInterface->CreateNode(TRUE)); 
  assert(0 == (yyval.node)->GetIndexWidth()); 
  assert(0 == (yyval.node)->GetValueWidth());
}
#line 1957 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 34:
#line 474 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode(GlobalParserInterface->CreateNode(FALSE)); 
  assert(0 == (yyval.node)->GetIndexWidth()); 
  assert(0 == (yyval.node)->GetValueWidth());
}
#line 1967 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 35:
#line 480 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = (yyvsp[0].node);
}
#line 1975 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 36:
#line 484 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  ASTNode * n = new ASTNode(GlobalParserInterface->CreateNode(EQ,*(yyvsp[-2].node), *(yyvsp[-1].node)));
  (yyval.node) = n;
  delete (yyvsp[-2].node);
  delete (yyvsp[-1].node);      
}
#line 1986 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 37:
#line 491 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  using namespace stp;

  ASTVec terms = *(yyvsp[-1].vec);
  ASTVec forms;

  for(ASTVec::const_iterator it=terms.begin(),itend=terms.end();
      it!=itend; it++) {
    for(ASTVec::const_iterator it2=it+1; it2!=itend; it2++) {
      ASTNode n = (GlobalParserInterface->nf->CreateNode(NOT, GlobalParserInterface->CreateNode(EQ, *it, *it2)));

          
      forms.push_back(n); 
    }
  }

  if(forms.size() == 0) 
    FatalError("empty distinct");
 
  (yyval.node) = (forms.size() == 1) ?
    new ASTNode(forms[0]) :
    new ASTNode(GlobalParserInterface->CreateNode(AND, forms));

  delete (yyvsp[-1].vec);
}
#line 2016 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 38:
#line 519 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  ASTNode * n = GlobalParserInterface->newNode(BVSLT, *(yyvsp[-2].node), *(yyvsp[-1].node));
  (yyval.node) = n;
  delete (yyvsp[-2].node);
  delete (yyvsp[-1].node);      
}
#line 2027 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 39:
#line 527 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  ASTNode * n = GlobalParserInterface->newNode(BVSLE, *(yyvsp[-2].node), *(yyvsp[-1].node));
  (yyval.node) = n;
  delete (yyvsp[-2].node);
  delete (yyvsp[-1].node);      
}
#line 2038 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 40:
#line 535 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  ASTNode * n = GlobalParserInterface->newNode(BVSGT, *(yyvsp[-2].node), *(yyvsp[-1].node));
  (yyval.node) = n;
  delete (yyvsp[-2].node);
  delete (yyvsp[-1].node);      
}
#line 2049 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 41:
#line 543 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  ASTNode * n = GlobalParserInterface->newNode(BVSGE, *(yyvsp[-2].node), *(yyvsp[-1].node));
  (yyval.node) = n;
  delete (yyvsp[-2].node);
  delete (yyvsp[-1].node);      
}
#line 2060 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 42:
#line 551 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  ASTNode * n = GlobalParserInterface->newNode(BVLT, *(yyvsp[-2].node), *(yyvsp[-1].node));
  (yyval.node) = n;
  delete (yyvsp[-2].node);
  delete (yyvsp[-1].node);      
}
#line 2071 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 43:
#line 559 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  ASTNode * n = GlobalParserInterface->newNode(BVLE, *(yyvsp[-2].node), *(yyvsp[-1].node));
  (yyval.node) = n;
  delete (yyvsp[-2].node);
  delete (yyvsp[-1].node);      
}
#line 2082 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 44:
#line 567 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  ASTNode * n = GlobalParserInterface->newNode(BVGT, *(yyvsp[-2].node), *(yyvsp[-1].node));
  (yyval.node) = n;
  delete (yyvsp[-2].node);
  delete (yyvsp[-1].node);      
}
#line 2093 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 45:
#line 575 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  ASTNode * n = GlobalParserInterface->newNode(BVGE, *(yyvsp[-2].node), *(yyvsp[-1].node));
  (yyval.node) = n;
  delete (yyvsp[-2].node);
  delete (yyvsp[-1].node);      
}
#line 2104 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 46:
#line 582 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = (yyvsp[-1].node);
}
#line 2112 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 47:
#line 586 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode(GlobalParserInterface->nf->CreateNode(NOT, *(yyvsp[-1].node)));
  delete (yyvsp[-1].node);
}
#line 2121 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 48:
#line 591 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = GlobalParserInterface->newNode(IMPLIES, *(yyvsp[-2].node), *(yyvsp[-1].node));
  delete (yyvsp[-2].node);
  delete (yyvsp[-1].node);
}
#line 2131 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 49:
#line 597 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode(GlobalParserInterface->nf->CreateNode(ITE, *(yyvsp[-3].node), *(yyvsp[-2].node), *(yyvsp[-1].node)));
  delete (yyvsp[-3].node);
  delete (yyvsp[-2].node);
  delete (yyvsp[-1].node);
}
#line 2142 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 50:
#line 604 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode(GlobalParserInterface->CreateNode(AND, *(yyvsp[-1].vec)));
  delete (yyvsp[-1].vec);
}
#line 2151 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 51:
#line 609 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode(GlobalParserInterface->CreateNode(OR, *(yyvsp[-1].vec)));
  delete (yyvsp[-1].vec);
}
#line 2160 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 52:
#line 614 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = GlobalParserInterface->newNode(XOR, *(yyvsp[-2].node), *(yyvsp[-1].node));
  delete (yyvsp[-2].node);
  delete (yyvsp[-1].node);
}
#line 2170 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 53:
#line 620 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = GlobalParserInterface->newNode(IFF, *(yyvsp[-2].node), *(yyvsp[-1].node));
  delete (yyvsp[-2].node);
  delete (yyvsp[-1].node);
}
#line 2180 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 54:
#line 627 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = (yyvsp[-1].node);
  GlobalParserInterface->letMgr->CleanupLetIDMap();                      
}
#line 2189 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 55:
#line 635 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  //populate the hashtable from LET-var -->
  //LET-exprs and then process them:
  //
  //1. ensure that LET variables do not clash
  //1. with declared variables.
  //
  //2. Ensure that LET variables are not
  //2. defined more than once
  GlobalParserInterface->letMgr->LetExprMgr(*(yyvsp[-2].str),*(yyvsp[-1].node));
  
  delete (yyvsp[-2].str);
  delete (yyvsp[-1].node);      
}
#line 2208 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 56:
#line 650 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  //Do LET-expr management
  GlobalParserInterface->letMgr->LetExprMgr(*(yyvsp[-2].str),*(yyvsp[-1].node));
  delete (yyvsp[-2].str);
  delete (yyvsp[-1].node);     
}
#line 2219 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 57:
#line 659 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.vec) = new ASTVec;
  if ((yyvsp[0].node) != NULL) {
    (yyval.vec)->push_back(*(yyvsp[0].node));
    delete (yyvsp[0].node);
  }
}
#line 2231 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 58:
#line 668 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  if ((yyvsp[-1].vec) != NULL && (yyvsp[0].node) != NULL) {
    (yyvsp[-1].vec)->push_back(*(yyvsp[0].node));
    (yyval.vec) = (yyvsp[-1].vec);
    delete (yyvsp[0].node);
  }
}
#line 2243 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 59:
#line 679 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode(GlobalParserInterface->CreateBVConst(*(yyvsp[0].str), 10, 32));
  delete (yyvsp[0].str);
}
#line 2252 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 60:
#line 684 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode(GlobalParserInterface->CreateBVConst(*(yyvsp[-3].str),10,(yyvsp[-1].uintval)));
  delete (yyvsp[-3].str);
}
#line 2261 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 61:
#line 689 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
(yyval.node) = (yyvsp[0].node);
}
#line 2269 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 62:
#line 695 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2275 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 63:
#line 697 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode((*(yyvsp[0].node)));
  delete (yyvsp[0].node);
}
#line 2284 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 64:
#line 702 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = (yyvsp[-1].node);
}
#line 2292 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 65:
#line 706 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  //ARRAY READ
  // valuewidth is same as array, indexwidth is 0.
  ASTNode array = *(yyvsp[-1].node);
  ASTNode index = *(yyvsp[0].node);
  unsigned int width = array.GetValueWidth();
  ASTNode * n = 
    new ASTNode(GlobalParserInterface->nf->CreateTerm(READ, width, array, index));
  (yyval.node) = n;
  delete (yyvsp[-1].node);
  delete (yyvsp[0].node);
}
#line 2309 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 66:
#line 719 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  //ARRAY WRITE
  unsigned int width = (yyvsp[0].node)->GetValueWidth();
  ASTNode array = *(yyvsp[-2].node);
  ASTNode index = *(yyvsp[-1].node);
  ASTNode writeval = *(yyvsp[0].node);
  ASTNode write_term = GlobalParserInterface->nf->CreateArrayTerm(WRITE,(yyvsp[-2].node)->GetIndexWidth(),width,array,index,writeval);
  ASTNode * n = new ASTNode(write_term);
  (yyval.node) = n;
  delete (yyvsp[-2].node);
  delete (yyvsp[-1].node);
  delete (yyvsp[0].node);
}
#line 2327 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 67:
#line 733 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  int width = (yyvsp[-4].uintval) - (yyvsp[-2].uintval) + 1;
  if (width < 0)
    yyerror("Negative width in extract");
      
  if((unsigned)(yyvsp[-4].uintval) >= (yyvsp[0].node)->GetValueWidth())
    yyerror("Parsing: Wrong width in BVEXTRACT\n");                      
      
  ASTNode hi  =  GlobalParserInterface->CreateBVConst(32, (yyvsp[-4].uintval));
  ASTNode low =  GlobalParserInterface->CreateBVConst(32, (yyvsp[-2].uintval));
  ASTNode output = GlobalParserInterface->nf->CreateTerm(BVEXTRACT, width, *(yyvsp[0].node),hi,low);
  ASTNode * n = new ASTNode(output);
  (yyval.node) = n;
  delete (yyvsp[0].node);
}
#line 2347 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 68:
#line 749 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  const unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  (yyval.node) = new ASTNode(GlobalParserInterface->nf->CreateArrayTerm(ITE,(yyvsp[0].node)->GetIndexWidth(), width,*(yyvsp[-2].node), *(yyvsp[-1].node), *(yyvsp[0].node)));      
  delete (yyvsp[-2].node);
  delete (yyvsp[-1].node);
  delete (yyvsp[0].node);
}
#line 2359 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 69:
#line 757 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  const unsigned int width = (yyvsp[-1].node)->GetValueWidth() + (yyvsp[0].node)->GetValueWidth();
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVCONCAT, width, *(yyvsp[-1].node), *(yyvsp[0].node)));
  (yyval.node) = n;
  delete (yyvsp[-1].node);
  delete (yyvsp[0].node);
}
#line 2371 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 70:
#line 765 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  //this is the BVNOT (term) in the CVCL language
  unsigned int width = (yyvsp[0].node)->GetValueWidth();
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVNOT, width, *(yyvsp[0].node)));
  (yyval.node) = n;
  delete (yyvsp[0].node);
}
#line 2383 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 71:
#line 773 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  //this is the BVUMINUS term in CVCL langauge
  unsigned width = (yyvsp[0].node)->GetValueWidth();
  ASTNode * n =  new ASTNode(GlobalParserInterface->nf->CreateTerm(BVUMINUS,width,*(yyvsp[0].node)));
  (yyval.node) = n;
  delete (yyvsp[0].node);
}
#line 2395 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 72:
#line 781 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(BVAND, width, *(yyvsp[-1].node), *(yyvsp[0].node));
  (yyval.node) = n;
  delete (yyvsp[-1].node);
  delete (yyvsp[0].node);
}
#line 2407 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 73:
#line 789 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(BVOR, width, *(yyvsp[-1].node), *(yyvsp[0].node)); 
  (yyval.node) = n;
  delete (yyvsp[-1].node);
  delete (yyvsp[0].node);
}
#line 2419 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 74:
#line 797 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n =GlobalParserInterface->newNode(BVXOR, width, *(yyvsp[-1].node), *(yyvsp[0].node));
  (yyval.node) = n;
  delete (yyvsp[-1].node);
  delete (yyvsp[0].node);
}
#line 2431 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 75:
#line 805 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
//   (bvxnor s t) abbreviates (bvor (bvand s t) (bvand (bvnot s) (bvnot t)))

	
      unsigned int width = (yyvsp[-1].node)->GetValueWidth();
      ASTNode * n = new ASTNode(
      GlobalParserInterface->nf->CreateTerm( BVOR, width,
     GlobalParserInterface->nf->CreateTerm(BVAND, width, *(yyvsp[-1].node), *(yyvsp[0].node)),
     GlobalParserInterface->nf->CreateTerm(BVAND, width,
	     GlobalParserInterface->nf->CreateTerm(BVNOT, width, *(yyvsp[-1].node)),
     	 GlobalParserInterface->nf->CreateTerm(BVNOT, width, *(yyvsp[0].node))
     )));

      (yyval.node) = n;
      delete (yyvsp[-1].node);
      delete (yyvsp[0].node);
  
  }
#line 2454 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 76:
#line 824 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
	

  	ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(ITE, 1, 
  	GlobalParserInterface->nf->CreateNode(EQ, *(yyvsp[-1].node), *(yyvsp[0].node)),
  	GlobalParserInterface->CreateOneConst(1),
  	GlobalParserInterface->CreateZeroConst(1)));
  	
      (yyval.node) = n;
      delete (yyvsp[-1].node);
      delete (yyvsp[0].node);
  }
#line 2471 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 77:
#line 839 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  const unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(BVSUB, width, *(yyvsp[-1].node), *(yyvsp[0].node));
  (yyval.node) = n;
  delete (yyvsp[-1].node);
  delete (yyvsp[0].node);
}
#line 2483 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 78:
#line 847 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  const unsigned int width = (*(yyvsp[0].vec))[0].GetValueWidth();
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVPLUS, width, *(yyvsp[0].vec)));
  (yyval.node) = n;
  delete (yyvsp[0].vec);

}
#line 2495 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 79:
#line 855 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  const unsigned int width = (*(yyvsp[0].vec))[0].GetValueWidth();
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVMULT, width, *(yyvsp[0].vec)));
  (yyval.node) = n;
  delete (yyvsp[0].vec);
}
#line 2506 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 80:
#line 863 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(BVDIV, width, *(yyvsp[-1].node), *(yyvsp[0].node));
  (yyval.node) = n;

  delete (yyvsp[-1].node);
  delete (yyvsp[0].node);
}
#line 2519 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 81:
#line 872 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(BVMOD, width, *(yyvsp[-1].node), *(yyvsp[0].node));
  (yyval.node) = n;

  delete (yyvsp[-1].node);
  delete (yyvsp[0].node);
}
#line 2532 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 82:
#line 881 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(SBVDIV, width, *(yyvsp[-1].node), *(yyvsp[0].node));
  (yyval.node) = n;

  delete (yyvsp[-1].node);
  delete (yyvsp[0].node);
}
#line 2545 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 83:
#line 890 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(SBVREM, width, *(yyvsp[-1].node), *(yyvsp[0].node));
  (yyval.node) = n;
  delete (yyvsp[-1].node);
  delete (yyvsp[0].node);
}
#line 2557 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 84:
#line 898 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(SBVMOD, width, *(yyvsp[-1].node), *(yyvsp[0].node));
  (yyval.node) = n;
  delete (yyvsp[-1].node);
  delete (yyvsp[0].node);
}
#line 2569 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 85:
#line 906 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVNOT, width, GlobalParserInterface->nf->CreateTerm(BVAND, width, *(yyvsp[-1].node), *(yyvsp[0].node))));
  (yyval.node) = n;
  delete (yyvsp[-1].node);
  delete (yyvsp[0].node);
}
#line 2581 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 86:
#line 914 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVNOT, width, GlobalParserInterface->nf->CreateTerm(BVOR, width, *(yyvsp[-1].node), *(yyvsp[0].node)))); 
  (yyval.node) = n;
  delete (yyvsp[-1].node);
  delete (yyvsp[0].node);
}
#line 2593 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 87:
#line 922 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  // shifting left by who know how much?
  unsigned int w = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(BVLEFTSHIFT,w,*(yyvsp[-1].node),*(yyvsp[0].node));
  (yyval.node) = n;
  delete (yyvsp[-1].node);
  delete (yyvsp[0].node);
}
#line 2606 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 88:
#line 931 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  // shifting right by who know how much?
  unsigned int w = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(BVRIGHTSHIFT,w,*(yyvsp[-1].node),*(yyvsp[0].node));
  (yyval.node) = n;
  delete (yyvsp[-1].node);
  delete (yyvsp[0].node);
}
#line 2619 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 89:
#line 940 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  // shifting arithmetic right by who know how much?
  unsigned int w = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(BVSRSHIFT,w,*(yyvsp[-1].node),*(yyvsp[0].node));
  (yyval.node) = n;
  delete (yyvsp[-1].node);
  delete (yyvsp[0].node);
}
#line 2632 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 90:
#line 949 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
      
  ASTNode *n;
  unsigned width = (yyvsp[0].node)->GetValueWidth();
  unsigned rotate = (yyvsp[-2].uintval);
  if (0 == rotate)
    {
      n = (yyvsp[0].node);
    }
  else if (rotate < width)
    {
      ASTNode high = GlobalParserInterface->CreateBVConst(32,width-1);
      ASTNode zero = GlobalParserInterface->CreateBVConst(32,0);
      ASTNode cut = GlobalParserInterface->CreateBVConst(32,width-rotate);
      ASTNode cutMinusOne = GlobalParserInterface->CreateBVConst(32,width-rotate-1);

      ASTNode top =  GlobalParserInterface->nf->CreateTerm(BVEXTRACT,rotate,*(yyvsp[0].node),high, cut);
      ASTNode bottom =  GlobalParserInterface->nf->CreateTerm(BVEXTRACT,width-rotate,*(yyvsp[0].node),cutMinusOne,zero);
      n =  new ASTNode(GlobalParserInterface->nf->CreateTerm(BVCONCAT,width,bottom,top));
      delete (yyvsp[0].node);
    }
  else
    {
      n = NULL; // remove gcc warning.
      yyerror("Rotate must be strictly less than the width.");
    }
      
  (yyval.node) = n;
      
}
#line 2667 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 91:
#line 980 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
      
  ASTNode *n;
  unsigned width = (yyvsp[0].node)->GetValueWidth();
  unsigned rotate = (yyvsp[-2].uintval);
  if (0 == rotate)
    {
      n = (yyvsp[0].node);
    }
  else if (rotate < width)
    {
      ASTNode high = GlobalParserInterface->CreateBVConst(32,width-1);
      ASTNode zero = GlobalParserInterface->CreateBVConst(32,0);
      ASTNode cut = GlobalParserInterface->CreateBVConst(32,rotate); 
      ASTNode cutMinusOne = GlobalParserInterface->CreateBVConst(32,rotate-1);

      ASTNode bottom =  GlobalParserInterface->nf->CreateTerm(BVEXTRACT,rotate,*(yyvsp[0].node),cutMinusOne, zero);
      ASTNode top =  GlobalParserInterface->nf->CreateTerm(BVEXTRACT,width-rotate,*(yyvsp[0].node),high,cut);
      n =  new ASTNode(GlobalParserInterface->nf->CreateTerm(BVCONCAT,width,bottom,top));
      delete (yyvsp[0].node);
    }
  else
    {
      n = NULL; // remove gcc warning.
      yyerror("Rotate must be strictly less than the width.");
    }
      
  (yyval.node) = n;
      
}
#line 2702 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 92:
#line 1011 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
	  unsigned count = (yyvsp[-2].uintval);
	  if (count < 1)
	  	FatalError("One or more repeats please");

	  unsigned w = (yyvsp[0].node)->GetValueWidth();  
      ASTNode n =  *(yyvsp[0].node);
      
      for (unsigned i =1; i < count; i++)
      {
      	  n = GlobalParserInterface->nf->CreateTerm(BVCONCAT,w*(i+1),n,*(yyvsp[0].node));
      }
       delete (yyvsp[0].node);
      (yyval.node) = new ASTNode(n);
    }
#line 2722 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 93:
#line 1027 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  unsigned w = (yyvsp[0].node)->GetValueWidth() + (yyvsp[-2].uintval);
  ASTNode width = GlobalParserInterface->CreateBVConst(32,w);
  ASTNode *n =  new ASTNode(GlobalParserInterface->nf->CreateTerm(BVSX,w,*(yyvsp[0].node),width));
  (yyval.node) = n;
  delete (yyvsp[0].node);
}
#line 2734 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 94:
#line 1035 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  unsigned w = (yyvsp[0].node)->GetValueWidth() + (yyvsp[-2].uintval);
  ASTNode width = GlobalParserInterface->CreateBVConst(32,w);
  ASTNode *n =  new ASTNode(GlobalParserInterface->nf->CreateTerm(BVZX,w,*(yyvsp[0].node),width));
  (yyval.node) = n;
  delete (yyvsp[0].node);
}
#line 2746 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 95:
#line 1046 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  // Just return BV width.  If sort is BOOL, width is 0.
  // Otherwise, BITVEC[w] returns w. 
  //
  //((indexwidth is 0) && (valuewidth>0)) iff type is BV
  (yyval.indexvaluewidth).indexwidth = 0;
  unsigned int length = (yyvsp[-1].uintval);
  if(length > 0) {
    (yyval.indexvaluewidth).valuewidth = length;
  }
  else {
    FatalError("Fatal Error: parsing: BITVECTORS must be of positive length: \n");
  }
}
#line 2765 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 96:
#line 1061 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  unsigned int index_len = (yyvsp[-3].uintval);
  unsigned int value_len = (yyvsp[-1].uintval);
  if(index_len > 0) {
    (yyval.indexvaluewidth).indexwidth = (yyvsp[-3].uintval);
  }
  else {
    FatalError("Fatal Error: parsing: BITVECTORS must be of positive length: \n");
  }

  if(value_len > 0) {
    (yyval.indexvaluewidth).valuewidth = (yyvsp[-1].uintval);
  }
  else {
    FatalError("Fatal Error: parsing: BITVECTORS must be of positive length: \n");
  }
}
#line 2787 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 97:
#line 1082 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode((*(yyvsp[0].node))); 
  delete (yyvsp[0].node);      
}
#line 2796 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 98:
#line 1087 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode((*(yyvsp[0].node)));
  delete (yyvsp[0].node);
}
#line 2805 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 99:
#line 1092 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = (yyvsp[0].node);
}
#line 2813 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 100:
#line 1099 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = (yyvsp[0].node); 
}
#line 2821 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;

  case 101:
#line 1103 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode((*(yyvsp[0].node))); 
  delete (yyvsp[0].node);      
}
#line 2830 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
    break;


#line 2834 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.cpp" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (AssertsQuery, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (AssertsQuery, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
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
                      yytoken, &yylval, AssertsQuery);
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

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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
                  yystos[yystate], yyvsp, AssertsQuery);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (AssertsQuery, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, AssertsQuery);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, AssertsQuery);
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
  return yyresult;
}
#line 1108 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1906  */

