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
#define yyparse         smt2parse
#define yylex           smt2lex
#define yyerror         smt2error
#define yydebug         smt2debug
#define yynerrs         smt2nerrs

#define yylval          smt2lval
#define yychar          smt2char

/* Copy the first part of user declarations.  */
#line 1 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:339  */

  /********************************************************************
   * AUTHORS:  Trevor Hansen
   *
   * BEGIN DATE: May, 2010
   *
   * This file is modified version of the STP's smtlib.y file. Please
   * see CVCL license below
  ********************************************************************/

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

  extern char* smt2text;
  extern int smt2lineno;
  extern int smt2lex(void);

  int yyerror(const char *s) {
    cout << "(error \"syntax error: line " << smt2lineno << " " << s << "  token: " << smt2text << "\")" << std::endl;
    return 1;
  }

   
#define YYLTYPE_IS_TRIVIAL 1
#define YYMAXDEPTH 104857600
#define YYERROR_VERBOSE 1
#define YY_EXIT_FAILURE -1

  

#line 150 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:339  */

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
   by #include "parsesmt2.hpp".  */
#ifndef YY_SMT2_HOME_MAP2CHECK_DEVEL_TOOL_MAP_SRC_ON_DOCKER_DEPENDENCIES_STP_LIB_PARSER_PARSESMT2_HPP_INCLUDED
# define YY_SMT2_HOME_MAP2CHECK_DEVEL_TOOL_MAP_SRC_ON_DOCKER_DEPENDENCIES_STP_LIB_PARSER_PARSESMT2_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int smt2debug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    END = 0,
    NUMERAL_TOK = 258,
    BVCONST_DECIMAL_TOK = 259,
    BVCONST_BINARY_TOK = 260,
    BVCONST_HEXIDECIMAL_TOK = 261,
    DECIMAL_TOK = 262,
    FORMID_TOK = 263,
    TERMID_TOK = 264,
    STRING_TOK = 265,
    BITVECTOR_FUNCTIONID_TOK = 266,
    BOOLEAN_FUNCTIONID_TOK = 267,
    SOURCE_TOK = 268,
    CATEGORY_TOK = 269,
    DIFFICULTY_TOK = 270,
    VERSION_TOK = 271,
    STATUS_TOK = 272,
    UNDERSCORE_TOK = 273,
    LPAREN_TOK = 274,
    RPAREN_TOK = 275,
    EXCLAIMATION_MARK_TOK = 276,
    NAMED_ATTRIBUTE_TOK = 277,
    BVLEFTSHIFT_1_TOK = 278,
    BVRIGHTSHIFT_1_TOK = 279,
    BVARITHRIGHTSHIFT_TOK = 280,
    BVPLUS_TOK = 281,
    BVSUB_TOK = 282,
    BVNOT_TOK = 283,
    BVMULT_TOK = 284,
    BVDIV_TOK = 285,
    SBVDIV_TOK = 286,
    BVMOD_TOK = 287,
    SBVREM_TOK = 288,
    SBVMOD_TOK = 289,
    BVNEG_TOK = 290,
    BVAND_TOK = 291,
    BVOR_TOK = 292,
    BVXOR_TOK = 293,
    BVNAND_TOK = 294,
    BVNOR_TOK = 295,
    BVXNOR_TOK = 296,
    BVCONCAT_TOK = 297,
    BVLT_TOK = 298,
    BVGT_TOK = 299,
    BVLE_TOK = 300,
    BVGE_TOK = 301,
    BVSLT_TOK = 302,
    BVSGT_TOK = 303,
    BVSLE_TOK = 304,
    BVSGE_TOK = 305,
    BVSX_TOK = 306,
    BVEXTRACT_TOK = 307,
    BVZX_TOK = 308,
    BVROTATE_RIGHT_TOK = 309,
    BVROTATE_LEFT_TOK = 310,
    BVREPEAT_TOK = 311,
    BVCOMP_TOK = 312,
    BITVEC_TOK = 313,
    ARRAY_TOK = 314,
    BOOL_TOK = 315,
    TRUE_TOK = 316,
    FALSE_TOK = 317,
    NOT_TOK = 318,
    AND_TOK = 319,
    OR_TOK = 320,
    XOR_TOK = 321,
    ITE_TOK = 322,
    EQ_TOK = 323,
    IMPLIES_TOK = 324,
    DISTINCT_TOK = 325,
    LET_TOK = 326,
    COLON_TOK = 327,
    ASSERT_TOK = 328,
    CHECK_SAT_TOK = 329,
    CHECK_SAT_ASSUMING_TOK = 330,
    DECLARE_CONST_TOK = 331,
    DECLARE_FUNCTION_TOK = 332,
    DECLARE_SORT_TOK = 333,
    DEFINE_FUNCTION_TOK = 334,
    DECLARE_FUN_REC_TOK = 335,
    DECLARE_FUNS_REC_TOK = 336,
    DEFINE_SORT_TOK = 337,
    ECHO_TOK = 338,
    EXIT_TOK = 339,
    GET_ASSERTIONS_TOK = 340,
    GET_ASSIGNMENT_TOK = 341,
    GET_INFO_TOK = 342,
    GET_MODEL_TOK = 343,
    GET_OPTION_TOK = 344,
    GET_PROOF_TOK = 345,
    GET_UNSAT_ASSUMPTION_TOK = 346,
    GET_UNSAT_CORE_TOK = 347,
    GET_VALUE_TOK = 348,
    POP_TOK = 349,
    PUSH_TOK = 350,
    RESET_TOK = 351,
    RESET_ASSERTIONS_TOK = 352,
    NOTES_TOK = 353,
    LOGIC_TOK = 354,
    SET_OPTION_TOK = 355,
    SELECT_TOK = 356,
    STORE_TOK = 357
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 76 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:355  */
  
  unsigned uintval;                  /* for numerals in types. */
  //ASTNode,ASTVec
  ASTNode *node;
  ASTVec *vec;
  std::string *str;

#line 302 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE smt2lval;

int smt2parse (void);

#endif /* !YY_SMT2_HOME_MAP2CHECK_DEVEL_TOOL_MAP_SRC_ON_DOCKER_DEPENDENCIES_STP_LIB_PARSER_PARSESMT2_HPP_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 319 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:358  */

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
#define YYFINAL  21
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   930

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  103
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  19
/* YYNRULES -- Number of rules.  */
#define YYNRULES  121
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  369

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   357

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
      95,    96,    97,    98,    99,   100,   101,   102
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   219,   219,   227,   228,   233,   240,   245,   250,   255,
     260,   265,   272,   279,   284,   290,   297,   303,   309,   316,
     323,   329,   340,   345,   348,   354,   365,   371,   380,   400,
     413,   422,   438,   445,   479,   497,   499,   501,   503,   505,
     512,   522,   530,   554,   563,   572,   581,   593,   602,   613,
     619,   625,   630,   637,   664,   688,   695,   702,   709,   716,
     723,   730,   737,   744,   748,   753,   759,   766,   771,   776,
     782,   789,   788,   797,   803,   809,   833,   834,   837,   851,
     869,   879,   890,   895,   899,   912,   926,   942,   950,   959,
     967,   975,   983,   991,   999,  1007,  1015,  1033,  1044,  1052,
    1061,  1069,  1078,  1087,  1096,  1104,  1112,  1120,  1128,  1137,
    1146,  1155,  1178,  1201,  1217,  1223,  1230,  1237,  1247,  1257,
    1276,  1275
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "$undefined", "NUMERAL_TOK",
  "BVCONST_DECIMAL_TOK", "BVCONST_BINARY_TOK", "BVCONST_HEXIDECIMAL_TOK",
  "DECIMAL_TOK", "FORMID_TOK", "TERMID_TOK", "STRING_TOK",
  "BITVECTOR_FUNCTIONID_TOK", "BOOLEAN_FUNCTIONID_TOK", "SOURCE_TOK",
  "CATEGORY_TOK", "DIFFICULTY_TOK", "VERSION_TOK", "STATUS_TOK",
  "UNDERSCORE_TOK", "LPAREN_TOK", "RPAREN_TOK", "EXCLAIMATION_MARK_TOK",
  "NAMED_ATTRIBUTE_TOK", "BVLEFTSHIFT_1_TOK", "BVRIGHTSHIFT_1_TOK",
  "BVARITHRIGHTSHIFT_TOK", "BVPLUS_TOK", "BVSUB_TOK", "BVNOT_TOK",
  "BVMULT_TOK", "BVDIV_TOK", "SBVDIV_TOK", "BVMOD_TOK", "SBVREM_TOK",
  "SBVMOD_TOK", "BVNEG_TOK", "BVAND_TOK", "BVOR_TOK", "BVXOR_TOK",
  "BVNAND_TOK", "BVNOR_TOK", "BVXNOR_TOK", "BVCONCAT_TOK", "BVLT_TOK",
  "BVGT_TOK", "BVLE_TOK", "BVGE_TOK", "BVSLT_TOK", "BVSGT_TOK",
  "BVSLE_TOK", "BVSGE_TOK", "BVSX_TOK", "BVEXTRACT_TOK", "BVZX_TOK",
  "BVROTATE_RIGHT_TOK", "BVROTATE_LEFT_TOK", "BVREPEAT_TOK", "BVCOMP_TOK",
  "BITVEC_TOK", "ARRAY_TOK", "BOOL_TOK", "TRUE_TOK", "FALSE_TOK",
  "NOT_TOK", "AND_TOK", "OR_TOK", "XOR_TOK", "ITE_TOK", "EQ_TOK",
  "IMPLIES_TOK", "DISTINCT_TOK", "LET_TOK", "COLON_TOK", "ASSERT_TOK",
  "CHECK_SAT_TOK", "CHECK_SAT_ASSUMING_TOK", "DECLARE_CONST_TOK",
  "DECLARE_FUNCTION_TOK", "DECLARE_SORT_TOK", "DEFINE_FUNCTION_TOK",
  "DECLARE_FUN_REC_TOK", "DECLARE_FUNS_REC_TOK", "DEFINE_SORT_TOK",
  "ECHO_TOK", "EXIT_TOK", "GET_ASSERTIONS_TOK", "GET_ASSIGNMENT_TOK",
  "GET_INFO_TOK", "GET_MODEL_TOK", "GET_OPTION_TOK", "GET_PROOF_TOK",
  "GET_UNSAT_ASSUMPTION_TOK", "GET_UNSAT_CORE_TOK", "GET_VALUE_TOK",
  "POP_TOK", "PUSH_TOK", "RESET_TOK", "RESET_ASSERTIONS_TOK", "NOTES_TOK",
  "LOGIC_TOK", "SET_OPTION_TOK", "SELECT_TOK", "STORE_TOK", "$accept",
  "cmd", "commands", "cmdi", "function_param", "function_params",
  "function_def", "status", "attribute", "var_decl", "an_mixed",
  "an_formulas", "an_formula", "$@1", "lets", "let", "an_terms", "an_term",
  "$@2", YY_NULLPTR
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
     355,   356,   357
};
# endif

#define YYPACT_NINF -262

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-262)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      -7,   194,    18,    15,   149,  -262,    -5,     1,    11,    28,
      29,  -262,  -262,    17,    30,    37,  -262,   150,    31,     4,
      47,  -262,  -262,   194,  -262,  -262,   310,  -262,  -262,  -262,
     828,   828,    66,  -262,    76,  -262,  -262,   680,  -262,  -262,
    -262,  -262,  -262,  -262,    89,     0,  -262,    93,  -262,    85,
     680,   149,   828,   828,   828,   828,   828,   828,   828,   828,
     149,   149,   149,   149,   149,   680,   149,   680,    87,    92,
    -262,  -262,  -262,  -262,   109,   727,   828,   828,   828,   828,
     828,   828,   828,   828,   828,   828,   828,   828,   828,   828,
     828,   828,   828,   828,   828,   828,   828,   149,   828,   828,
      99,   119,   120,   -11,   393,   473,  -262,  -262,  -262,  -262,
    -262,  -262,     9,  -262,   553,   121,   828,   828,   828,   828,
     828,   828,   828,   828,   124,    88,  -262,    90,   149,   149,
     149,   828,   149,   134,   781,  -262,  -262,  -262,   138,   680,
      26,   828,   128,   135,   828,   828,   828,   828,   828,  -262,
     828,   828,   828,   828,   828,   828,  -262,   828,   828,   828,
     828,   828,   828,   828,   828,   828,   828,   828,  -262,  -262,
     -17,   146,   -16,  -262,    -3,   680,   149,   139,  -262,  -262,
    -262,  -262,  -262,  -262,  -262,   152,   140,   151,   154,   155,
     156,   157,   159,   160,  -262,  -262,  -262,  -262,   161,   149,
     170,   171,   172,  -262,  -262,  -262,   153,  -262,   633,   166,
     167,   190,   191,   195,   196,   137,  -262,  -262,  -262,  -262,
    -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,
    -262,  -262,  -262,  -262,  -262,  -262,  -262,   828,  -262,   828,
      13,  -262,   178,    14,   149,    23,  -262,   680,  -262,   180,
    -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,   181,
    -262,  -262,  -262,   192,   183,   153,  -262,   184,   202,   186,
     188,   193,   198,   204,   153,  -262,  -262,   158,   201,   197,
     163,   203,  -262,    79,   149,  -262,  -262,   680,   149,  -262,
     828,   205,   828,   828,   828,   828,   208,   209,   220,   206,
     173,   227,   214,   175,   215,  -262,   217,   218,   219,  -262,
     828,  -262,  -262,  -262,  -262,  -262,   828,   221,   177,   237,
     222,   185,   241,   228,  -262,  -262,  -262,  -262,   225,  -262,
     245,   233,   828,   253,   238,   216,  -262,   239,   246,  -262,
     252,   828,   258,   256,  -262,   257,  -262,   259,   263,   265,
     266,   226,   242,   273,   292,   293,   243,   279,   283,   301,
     285,   286,   287,  -262,   828,   288,  -262,   828,  -262
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     6,     0,     0,     0,     0,
       0,    12,    13,     0,     0,     0,    20,     0,     0,     0,
       0,     1,     2,     0,    51,    74,     0,    49,    50,     5,
       0,     0,     0,     9,     0,    10,    11,     0,    19,    18,
      35,    36,    37,    38,     0,    24,    21,     0,     4,     0,
      74,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     116,   115,    82,   118,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    43,    44,    34,    39,
      23,    22,     0,     3,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    47,     0,     0,     0,
       0,     0,     0,     0,     0,    80,    71,    63,     0,   118,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    91,
       0,     0,     0,     0,     0,     0,    92,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     7,     8,
       0,     0,     0,    26,     0,     0,     0,     0,    14,    45,
      46,    15,    17,    16,    73,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    64,    67,    48,    68,     0,     0,
       0,     0,     0,    54,    53,    81,     0,   114,     0,     0,
       0,     0,     0,     0,     0,     0,   120,    83,   108,   109,
     110,    99,    98,   100,   101,   103,   102,   104,   105,    93,
      94,    95,   106,   107,    96,    90,    97,     0,    84,     0,
       0,    41,     0,     0,     0,     0,    27,     0,    71,     0,
      59,    61,    60,    62,    55,    57,    56,    58,    69,     0,
      70,    52,    65,     0,     0,    77,   117,     0,     0,     0,
       0,     0,     0,     0,     0,    89,    85,     0,     0,     0,
       0,     0,    30,     0,     0,    75,    66,     0,     0,    76,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,     0,    88,
       0,    87,   112,   111,   113,   119,     0,     0,     0,     0,
       0,     0,     0,     0,    78,    79,    72,    86,     0,    40,
       0,     0,     0,     0,     0,     0,   121,     0,     0,    31,
       0,     0,     0,     0,    25,     0,    28,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    42,     0,     0,    33,     0,    32
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -262,  -262,  -262,   189,   136,  -262,  -262,  -262,  -262,  -262,
     -45,   -56,   122,  -262,  -261,  -262,  -262,   -30,  -262
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,    20,   173,   174,    35,   109,    45,    33,
     105,   125,   106,   206,   264,   265,   134,   107,   274
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint16 yytable[] =
{
     100,   101,   240,   243,   289,   114,   127,   110,   171,   172,
     111,   133,     1,   297,    30,    22,   171,   245,    21,   181,
      31,    32,   116,   117,   118,   119,   120,   121,   122,   123,
     138,   277,   280,    38,    23,   131,    37,   135,    34,    36,
      39,    46,   283,   241,   244,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,   159,   160,   161,   162,   163,   164,    48,   166,   167,
     182,   183,   278,   281,   143,   180,    47,   209,   210,   211,
     212,   213,   214,   284,   180,   102,   186,   187,   188,   189,
     190,   191,   192,   193,   208,   103,    24,   303,    24,   108,
      25,   201,    25,   112,   205,   113,   136,    26,   195,    26,
     197,   215,   137,   138,   218,   219,   220,   221,   222,   168,
     223,   224,   225,   226,   227,   228,    29,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   304,   169,
     170,   207,    24,   185,   194,   215,    25,   216,    69,    27,
      28,    27,    28,    26,   203,   217,   242,    24,   248,   273,
     250,    25,   249,    40,    41,    42,    43,    44,    26,   267,
     268,   251,   263,   115,   252,   253,   254,   255,   180,   256,
     257,   258,   124,   126,   126,   128,   129,   130,   132,   126,
     260,   261,   262,   269,   270,    27,    28,   279,   271,   272,
     285,   286,   287,   288,   290,   291,   292,   275,   293,   276,
      27,    28,    49,   294,   296,   300,   298,   237,   295,   165,
     299,   301,   302,   317,   318,   310,    69,   179,   315,   316,
     320,   319,   321,   322,   323,   330,   179,   324,   325,   326,
     331,   329,   332,   333,   334,   336,   335,   196,   337,   196,
     198,   199,   200,   338,   202,   196,   340,   307,   341,   343,
     309,   347,   311,   312,   313,   314,   344,     4,     5,     6,
       7,     8,   345,     9,   342,   348,   349,    10,    11,   350,
     327,   351,    12,   352,   354,   353,   328,    13,    14,    15,
      16,   356,    17,    18,    19,   357,   358,   115,   247,   360,
     355,   359,   339,   361,   362,   363,   364,   365,   367,     0,
     246,   346,     0,     0,     0,     0,     0,     0,    24,     0,
       0,   259,    50,     0,     0,     0,     0,     0,     0,    26,
     179,    51,     0,     0,   366,     0,     0,   368,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    52,    53,    54,    55,    56,    57,    58,
      59,     0,     0,     0,     0,     0,   282,     0,     0,   199,
       0,    27,    28,    60,    61,    62,    63,    64,    65,    66,
      67,    68,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    70,    71,
       0,    24,    72,     0,   139,    50,   305,     0,     0,   306,
     308,   140,   104,     0,   175,     0,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    52,    53,    54,    55,
      56,    57,    58,    59,     0,     0,     0,     0,     0,     0,
      96,     0,     0,     0,    27,    28,    60,    61,    62,    63,
     176,    65,    66,    67,   177,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    70,    71,
       0,    24,    72,     0,    73,    25,     0,     0,     0,     0,
       0,    74,   104,   178,    98,    99,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      96,     0,     0,     0,    27,    28,     0,     0,     0,     0,
      97,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    70,    71,
       0,    24,    72,     0,    73,    25,     0,     0,     0,     0,
       0,    74,   104,   184,    98,    99,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      96,     0,     0,     0,    27,    28,     0,     0,     0,     0,
      97,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    70,    71,
       0,    24,    72,     0,    73,    25,     0,     0,     0,     0,
       0,    74,   104,   266,    98,    99,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    70,    71,     0,    24,    72,
      96,    73,    25,     0,    27,    28,     0,     0,    74,   104,
      97,     0,     0,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    70,    71,    98,    99,    72,    96,   139,     0,
       0,    27,    28,     0,     0,   140,    75,    97,   141,     0,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    98,    99,     0,    96,     0,    70,    71,     0,     0,
      72,     0,    73,     0,    97,     0,     0,     0,   142,    74,
      75,   204,     0,     0,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,     0,     0,     0,     0,    98,    99,
       0,     0,     0,    70,    71,     0,     0,    72,    96,    73,
       0,     0,     0,     0,     0,     0,    74,    75,    97,     0,
       0,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    98,    99,     0,    96,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    97,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    98,
      99
};

static const yytype_int16 yycheck[] =
{
      30,    31,    19,    19,   265,    50,    62,     7,    19,    20,
      10,    67,    19,   274,    19,     0,    19,    20,     0,    10,
      19,    10,    52,    53,    54,    55,    56,    57,    58,    59,
       4,    18,    18,     3,    19,    65,    19,    67,    10,    10,
       3,    10,    19,    60,    60,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    20,    98,    99,
      61,    62,    59,    59,   104,   105,    72,    51,    52,    53,
      54,    55,    56,    60,   114,    19,   116,   117,   118,   119,
     120,   121,   122,   123,   139,    19,     8,    18,     8,    10,
      12,   131,    12,    10,   134,    20,    19,    19,    20,    19,
      20,   141,    20,     4,   144,   145,   146,   147,   148,    20,
     150,   151,   152,   153,   154,   155,     4,   157,   158,   159,
     160,   161,   162,   163,   164,   165,   166,   167,    59,    20,
      20,     3,     8,    22,    20,   175,    12,    19,    26,    61,
      62,    61,    62,    19,    20,    20,    10,     8,    19,    22,
      20,    12,    10,    13,    14,    15,    16,    17,    19,     3,
       3,    20,    19,    51,    20,    20,    20,    20,   208,    20,
      20,    20,    60,    61,    62,    63,    64,    65,    66,    67,
      20,    20,    20,     3,     3,    61,    62,    19,     3,     3,
      20,    20,    10,    20,    20,     3,    20,   237,    20,   239,
      61,    62,    23,    20,    10,    18,    58,   247,    20,    97,
      19,    58,    19,     3,    18,    20,   104,   105,    20,    20,
       3,    58,    18,    58,    19,    58,   114,    20,    20,    20,
       3,    20,    20,    58,     3,    20,    18,   125,     3,   127,
     128,   129,   130,    20,   132,   133,     3,   287,    20,    20,
     290,     3,   292,   293,   294,   295,    20,    73,    74,    75,
      76,    77,    20,    79,    58,    19,    19,    83,    84,    20,
     310,    18,    88,    18,    58,    19,   316,    93,    94,    95,
      96,    18,    98,    99,   100,     3,     3,   175,   176,    20,
      58,    58,   332,    20,     3,    20,    20,    20,    20,    -1,
     174,   341,    -1,    -1,    -1,    -1,    -1,    -1,     8,    -1,
      -1,   199,    12,    -1,    -1,    -1,    -1,    -1,    -1,    19,
     208,    21,    -1,    -1,   364,    -1,    -1,   367,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    43,    44,    45,    46,    47,    48,    49,
      50,    -1,    -1,    -1,    -1,    -1,   244,    -1,    -1,   247,
      -1,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     5,     6,
      -1,     8,     9,    -1,    11,    12,   284,    -1,    -1,   287,
     288,    18,    19,    -1,    21,    -1,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    -1,    -1,    -1,    -1,    -1,    -1,
      57,    -1,    -1,    -1,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     5,     6,
      -1,     8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,
      -1,    18,    19,    20,   101,   102,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      57,    -1,    -1,    -1,    61,    62,    -1,    -1,    -1,    -1,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     5,     6,
      -1,     8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,
      -1,    18,    19,    20,   101,   102,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      57,    -1,    -1,    -1,    61,    62,    -1,    -1,    -1,    -1,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     5,     6,
      -1,     8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,
      -1,    18,    19,    20,   101,   102,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     5,     6,    -1,     8,     9,
      57,    11,    12,    -1,    61,    62,    -1,    -1,    18,    19,
      67,    -1,    -1,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     5,     6,   101,   102,     9,    57,    11,    -1,
      -1,    61,    62,    -1,    -1,    18,    19,    67,    21,    -1,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   101,   102,    -1,    57,    -1,     5,     6,    -1,    -1,
       9,    -1,    11,    -1,    67,    -1,    -1,    -1,    71,    18,
      19,    20,    -1,    -1,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    -1,    -1,    -1,    -1,   101,   102,
      -1,    -1,    -1,     5,     6,    -1,    -1,     9,    57,    11,
      -1,    -1,    -1,    -1,    -1,    -1,    18,    19,    67,    -1,
      -1,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   101,   102,    -1,    57,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   101,
     102
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    19,   104,   105,    73,    74,    75,    76,    77,    79,
      83,    84,    88,    93,    94,    95,    96,    98,    99,   100,
     106,     0,     0,    19,     8,    12,    19,    61,    62,   115,
      19,    19,    10,   112,    10,   109,    10,    19,     3,     3,
      13,    14,    15,    16,    17,   111,    10,    72,    20,   106,
      12,    21,    43,    44,    45,    46,    47,    48,    49,    50,
      63,    64,    65,    66,    67,    68,    69,    70,    71,   115,
       5,     6,     9,    11,    18,    19,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    57,    67,   101,   102,
     120,   120,    19,    19,    19,   113,   115,   120,    10,   110,
       7,    10,    10,    20,   113,   115,   120,   120,   120,   120,
     120,   120,   120,   120,   115,   114,   115,   114,   115,   115,
     115,   120,   115,   114,   119,   120,    19,    20,     4,    11,
      18,    21,    71,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   115,   120,   120,    20,    20,
      20,    19,    20,   107,   108,    21,    67,    71,    20,   115,
     120,    10,    61,    62,    20,    22,   120,   120,   120,   120,
     120,   120,   120,   120,    20,    20,   115,    20,   115,   115,
     115,   120,   115,    20,    20,   120,   116,     3,   113,    51,
      52,    53,    54,    55,    56,   120,    19,    20,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
      19,    60,    10,    19,    60,    20,   107,   115,    19,    10,
      20,    20,    20,    20,    20,    20,    20,    20,    20,   115,
      20,    20,    20,    19,   117,   118,    20,     3,     3,     3,
       3,     3,     3,    22,   121,   120,   120,    18,    59,    19,
      18,    59,   115,    19,    60,    20,    20,    10,    20,   117,
      20,     3,    20,    20,    20,    20,    10,   117,    58,    19,
      18,    58,    19,    18,    59,   115,   115,   120,   115,   120,
      20,   120,   120,   120,   120,    20,    20,     3,    18,    58,
       3,    18,    58,    19,    20,    20,    20,   120,   120,    20,
      58,     3,    20,    58,     3,    18,    20,     3,    20,   120,
       3,    20,    58,    20,    20,    20,   120,     3,    19,    19,
      20,    18,    18,    19,    58,    58,    18,     3,     3,    58,
      20,    20,     3,    20,    20,    20,   120,    20,   120
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   103,   104,   105,   105,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   106,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   106,   107,   108,   108,   109,   109,
     109,   109,   109,   109,   110,   111,   111,   111,   111,   111,
     112,   112,   112,   113,   113,   113,   113,   114,   114,   115,
     115,   115,   115,   115,   115,   115,   115,   115,   115,   115,
     115,   115,   115,   115,   115,   115,   115,   115,   115,   115,
     115,   116,   115,   115,   115,   115,   117,   117,   118,   118,
     119,   119,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     121,   120
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     4,     3,     2,     1,     4,     4,     2,
       2,     2,     1,     1,     4,     4,     4,     4,     2,     2,
       1,     2,     3,     3,     2,     8,     1,     2,    10,     6,
       5,     9,    18,    17,     1,     1,     1,     1,     1,     2,
       8,     4,    16,     1,     1,     2,     2,     1,     2,     1,
       1,     1,     5,     4,     4,     5,     5,     5,     5,     5,
       5,     5,     5,     3,     4,     5,     6,     4,     4,     5,
       5,     0,     8,     4,     1,     6,     2,     1,     4,     4,
       1,     2,     1,     3,     3,     4,     7,     6,     6,     4,
       3,     2,     2,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     6,     6,     6,     3,     1,     1,     4,     1,     6,
       0,     8
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
      yyerror (YY_("syntax error: cannot back up")); \
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
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
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
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
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
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
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
yyparse (void)
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
#line 220 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
       GlobalParserInterface->cleanUp();
       YYACCEPT;
}
#line 1766 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 4:
#line 229 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {}
#line 1772 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 5:
#line 234 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
      GlobalParserInterface->AddAssert(*(yyvsp[0].node));
      GlobalParserInterface->deleteNode((yyvsp[0].node));
      GlobalParserInterface->success();
    }
#line 1782 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 6:
#line 241 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
      GlobalParserInterface->checkSat(GlobalParserInterface->getAssertVector());
    }
#line 1790 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 7:
#line 246 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
      GlobalParserInterface->unsupported();
    }
#line 1798 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 8:
#line 251 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
      GlobalParserInterface->unsupported();
    }
#line 1806 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 9:
#line 256 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
      GlobalParserInterface->success();
    }
#line 1814 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 10:
#line 261 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
      GlobalParserInterface->success();
    }
#line 1822 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 11:
#line 266 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
      std::cout << "\"" << *(yyvsp[0].str)  << "\"" << std::endl;
      delete (yyvsp[0].str);
      GlobalParserInterface->success();
    }
#line 1832 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 12:
#line 273 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
       GlobalParserInterface->cleanUp();
       GlobalParserInterface->success();
       YYACCEPT;
    }
#line 1842 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 13:
#line 280 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
       GlobalParserInterface->getModel();
    }
#line 1850 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 14:
#line 285 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
      GlobalParserInterface->getValue(*(yyvsp[-1].vec));
      delete (yyvsp[-1].vec);
    }
#line 1859 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 15:
#line 291 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
       GlobalParserInterface->setOption(*(yyvsp[-1].str),*(yyvsp[0].str));
       delete (yyvsp[-1].str);
       delete (yyvsp[0].str);
    }
#line 1869 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 16:
#line 298 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
       GlobalParserInterface->setOption(*(yyvsp[-1].str),"false");
       delete (yyvsp[-1].str);
    }
#line 1878 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 17:
#line 304 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
       GlobalParserInterface->setOption(*(yyvsp[-1].str),"true");
       delete (yyvsp[-1].str);
    }
#line 1887 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 18:
#line 310 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
        for (unsigned i=0; i < (yyvsp[0].uintval);i++)
            GlobalParserInterface->push();
        GlobalParserInterface->success();
    }
#line 1897 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 19:
#line 317 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
        for (unsigned i=0; i < (yyvsp[0].uintval);i++)
            GlobalParserInterface->pop();
        GlobalParserInterface->success();
    }
#line 1907 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 20:
#line 324 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
       GlobalParserInterface->reset();
       GlobalParserInterface->success();
    }
#line 1916 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 21:
#line 330 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
      if (!(0 == strcmp((yyvsp[0].str)->c_str(),"QF_BV") ||
            0 == strcmp((yyvsp[0].str)->c_str(),"QF_ABV") ||
            0 == strcmp((yyvsp[0].str)->c_str(),"QF_AUFBV"))) {
        yyerror("Wrong input logic");
      }
      GlobalParserInterface->success();
      delete (yyvsp[0].str);
    }
#line 1930 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 22:
#line 341 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
      delete (yyvsp[0].str);
    }
#line 1938 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 23:
#line 346 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {}
#line 1944 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 24:
#line 349 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {}
#line 1950 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 25:
#line 355 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode(GlobalParserInterface->LookupOrCreateSymbol((yyvsp[-6].str)->c_str()));
  GlobalParserInterface->addSymbol(*(yyval.node));
  (yyval.node)->SetIndexWidth(0);
  (yyval.node)->SetValueWidth((yyvsp[-2].uintval));
  delete (yyvsp[-6].str);
}
#line 1962 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 26:
#line 366 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.vec) = new ASTVec;
  (yyval.vec)->push_back(*(yyvsp[0].node));
  delete (yyvsp[0].node);
}
#line 1972 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 27:
#line 372 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.vec) = (yyvsp[-1].vec);
  (yyval.vec)->push_back(*(yyvsp[0].node));
  delete (yyvsp[0].node);
}
#line 1982 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 28:
#line 381 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  if ((yyvsp[0].node)->GetValueWidth() != (yyvsp[-2].uintval))
  {
    char msg [100];
    sprintf(msg, "Different bit-widths specified: %d %d", (yyvsp[0].node)->GetValueWidth(), (yyvsp[-2].uintval));
    yyerror(msg);
  }

  GlobalParserInterface->storeFunction(*(yyvsp[-9].str), *(yyvsp[-7].vec), *(yyvsp[0].node));

  // Next time the variable is used, we want it to be fresh.
  for (size_t i = 0; i < (yyvsp[-7].vec)->size(); i++)
    GlobalParserInterface->removeSymbol((*(yyvsp[-7].vec))[i]);

  delete (yyvsp[-9].str);
  delete (yyvsp[-7].vec);
  delete (yyvsp[0].node);
}
#line 2005 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 29:
#line 401 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  GlobalParserInterface->storeFunction(*(yyvsp[-5].str), *(yyvsp[-3].vec), *(yyvsp[0].node));

  // Next time the variable is used, we want it to be fresh.
  for (size_t i = 0; i < (yyvsp[-3].vec)->size(); i++)
   GlobalParserInterface->removeSymbol((*(yyvsp[-3].vec))[i]);

  delete (yyvsp[-5].str);
  delete (yyvsp[-3].vec);
  delete (yyvsp[0].node);
}
#line 2021 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 30:
#line 414 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  ASTVec empty;
  GlobalParserInterface->storeFunction(*(yyvsp[-4].str), empty, *(yyvsp[0].node));

  delete (yyvsp[-4].str);
  delete (yyvsp[0].node);
}
#line 2033 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 31:
#line 423 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  if ((yyvsp[0].node)->GetValueWidth() != (yyvsp[-2].uintval))
  {
    char msg [100];
    sprintf(msg, "Different bit-widths specified: %d %d", (yyvsp[0].node)->GetValueWidth(), (yyvsp[-2].uintval));
    yyerror(msg);
  }

  ASTVec empty;
  GlobalParserInterface->storeFunction(*(yyvsp[-8].str),empty, *(yyvsp[0].node));

  delete (yyvsp[-8].str);
  delete (yyvsp[0].node);
}
#line 2052 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 32:
#line 439 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  GlobalParserInterface->unsupported();
  delete (yyvsp[-17].str);
  delete (yyvsp[0].node);
}
#line 2062 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 33:
#line 446 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  GlobalParserInterface->unsupported();
  delete (yyvsp[-16].str);
  delete (yyvsp[0].node);

#if 0
  ASTNode s = GlobalParserInterface->LookupOrCreateSymbol((yyvsp[-16].str)->c_str());
  GlobalParserInterface->addSymbol(s);
  unsigned int index_len = (yyvsp[-8].uintval);
  unsigned int value_len = (yyvsp[-3].uintval);
  if(index_len > 0) {
    s.SetIndexWidth((yyvsp[-8].uintval));
  }
  else {
    FatalError("Fatal Error: parsing: BITVECTORS must be of positive length: \n");
  }

  if(value_len > 0) {
    s.SetValueWidth((yyvsp[-3].uintval));
  }
  else {
    FatalError("Fatal Error: parsing: BITVECTORS must be of positive length: \n");
  }

  ASTVec empty;
  GlobalParserInterface->storeFunction(*(yyvsp[-16].str),empty, *(yyvsp[0].node));
#endif 

}
#line 2096 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 34:
#line 479 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    { 
 
 std::transform((yyvsp[0].str)->begin(), (yyvsp[0].str)->end(), (yyvsp[0].str)->begin(), ::tolower);
  
  if (0 == strcmp((yyvsp[0].str)->c_str(), "sat"))
      input_status = TO_BE_SATISFIABLE;
  else if (0 == strcmp((yyvsp[0].str)->c_str(), "unsat"))
    input_status = TO_BE_UNSATISFIABLE;
  else if (0 == strcmp((yyvsp[0].str)->c_str(), "unknown"))
      input_status = TO_BE_UNKNOWN;
  else 
      yyerror((yyvsp[0].str)->c_str());
  delete (yyvsp[0].str);
  (yyval.node) = NULL; 
}
#line 2116 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 35:
#line 498 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {}
#line 2122 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 36:
#line 500 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {}
#line 2128 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 37:
#line 502 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {}
#line 2134 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 38:
#line 504 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {}
#line 2140 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 39:
#line 506 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {}
#line 2146 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 40:
#line 513 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  ASTNode s = GlobalParserInterface->LookupOrCreateSymbol((yyvsp[-7].str)->c_str());
  GlobalParserInterface->addSymbol(s);
  //Sort_symbs has the indexwidth/valuewidth. Set those fields in
  //var
  s.SetIndexWidth(0);
  s.SetValueWidth((yyvsp[-1].uintval));
  delete (yyvsp[-7].str);
}
#line 2160 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 41:
#line 523 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  ASTNode s = GlobalParserInterface->LookupOrCreateSymbol((yyvsp[-3].str)->c_str());
  s.SetIndexWidth(0);
  s.SetValueWidth(0);
  GlobalParserInterface->addSymbol(s);
  delete (yyvsp[-3].str);
}
#line 2172 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 42:
#line 531 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  ASTNode s = GlobalParserInterface->LookupOrCreateSymbol((yyvsp[-15].str)->c_str());
  GlobalParserInterface->addSymbol(s);
  unsigned int index_len = (yyvsp[-7].uintval);
  unsigned int value_len = (yyvsp[-2].uintval);
  if(index_len > 0) {
    s.SetIndexWidth((yyvsp[-7].uintval));
  }
  else {
    FatalError("Fatal Error: parsing: BITVECTORS must be of positive length: \n");
  }

  if(value_len > 0) {
    s.SetValueWidth((yyvsp[-2].uintval));
  }
  else {
    FatalError("Fatal Error: parsing: BITVECTORS must be of positive length: \n");
  }
  delete (yyvsp[-15].str);
}
#line 2197 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 43:
#line 555 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.vec) = new ASTVec;
  if ((yyvsp[0].node) != NULL) {
    (yyval.vec)->push_back(*(yyvsp[0].node));
    GlobalParserInterface->deleteNode((yyvsp[0].node));
  }
}
#line 2209 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 44:
#line 564 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.vec) = new ASTVec;
  if ((yyvsp[0].node) != NULL) {
    (yyval.vec)->push_back(*(yyvsp[0].node));
    GlobalParserInterface->deleteNode((yyvsp[0].node));
  }
}
#line 2221 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 45:
#line 573 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  if ((yyvsp[-1].vec) != NULL && (yyvsp[0].node) != NULL) {
    (yyvsp[-1].vec)->push_back(*(yyvsp[0].node));
    (yyval.vec) = (yyvsp[-1].vec);
    GlobalParserInterface->deleteNode((yyvsp[0].node));
  }
}
#line 2233 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 46:
#line 582 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  if ((yyvsp[-1].vec) != NULL && (yyvsp[0].node) != NULL) {
    (yyvsp[-1].vec)->push_back(*(yyvsp[0].node));
    (yyval.vec) = (yyvsp[-1].vec);
    GlobalParserInterface->deleteNode((yyvsp[0].node));
  }
}
#line 2245 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 47:
#line 594 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.vec) = new ASTVec;
  if ((yyvsp[0].node) != NULL) {
    (yyval.vec)->push_back(*(yyvsp[0].node));
    GlobalParserInterface->deleteNode((yyvsp[0].node));
  }
}
#line 2257 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 48:
#line 603 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  if ((yyvsp[-1].vec) != NULL && (yyvsp[0].node) != NULL) {
    (yyvsp[-1].vec)->push_back(*(yyvsp[0].node));
    (yyval.vec) = (yyvsp[-1].vec);
    GlobalParserInterface->deleteNode((yyvsp[0].node));
  }
}
#line 2269 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 49:
#line 614 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.node) = GlobalParserInterface->newNode(GlobalParserInterface->CreateNode(TRUE)); 
  assert(0 == (yyval.node)->GetIndexWidth()); 
  assert(0 == (yyval.node)->GetValueWidth());
}
#line 2279 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 50:
#line 620 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.node) = GlobalParserInterface->newNode(GlobalParserInterface->CreateNode(FALSE)); 
  assert(0 == (yyval.node)->GetIndexWidth()); 
  assert(0 == (yyval.node)->GetValueWidth());
}
#line 2289 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 51:
#line 626 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.node) = GlobalParserInterface->newNode(*(yyvsp[0].node)); 
  GlobalParserInterface->deleteNode((yyvsp[0].node));      
}
#line 2298 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 52:
#line 631 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  ASTNode * n = GlobalParserInterface->newNode(EQ,*(yyvsp[-2].node), *(yyvsp[-1].node));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode((yyvsp[-2].node));
  GlobalParserInterface->deleteNode((yyvsp[-1].node));      
}
#line 2309 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 53:
#line 638 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  using namespace stp;

  ASTVec terms = *(yyvsp[-1].vec);
  ASTVec forms;

  for(ASTVec::const_iterator it=terms.begin(),itend=terms.end();
      it!=itend; it++)
  {
    for(ASTVec::const_iterator it2=it+1; it2!=itend; it2++) {
      ASTNode n =
        GlobalParserInterface->nf->CreateNode(NOT, GlobalParserInterface->CreateNode(EQ, *it, *it2));

      forms.push_back(n); 
    }
  }

  if(forms.size() == 0) 
    FatalError("empty distinct");
 
  (yyval.node) = (forms.size() == 1) ?
    GlobalParserInterface->newNode(forms[0]) :
    GlobalParserInterface->newNode(GlobalParserInterface->CreateNode(AND, forms));

  delete (yyvsp[-1].vec);
}
#line 2340 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 54:
#line 665 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  using namespace stp;

  ASTVec terms = *(yyvsp[-1].vec);
  ASTVec forms;

  for(ASTVec::const_iterator it=terms.begin(),itend=terms.end();
      it!=itend; it++) {
    for(ASTVec::const_iterator it2=it+1; it2!=itend; it2++) {
      ASTNode n = (GlobalParserInterface->nf->CreateNode(NOT, GlobalParserInterface->CreateNode(IFF, *it, *it2)));
      forms.push_back(n); 
    }
  }

  if(forms.size() == 0) 
    FatalError("empty distinct");
 
  (yyval.node) = (forms.size() == 1) ?
    GlobalParserInterface->newNode(forms[0]) :
    GlobalParserInterface->newNode(GlobalParserInterface->CreateNode(AND, forms));

  delete (yyvsp[-1].vec);
}
#line 2368 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 55:
#line 689 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  ASTNode * n = GlobalParserInterface->newNode(BVSLT, *(yyvsp[-2].node), *(yyvsp[-1].node));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode((yyvsp[-2].node));
  GlobalParserInterface->deleteNode((yyvsp[-1].node));      
}
#line 2379 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 56:
#line 696 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  ASTNode * n = GlobalParserInterface->newNode(BVSLE, *(yyvsp[-2].node), *(yyvsp[-1].node));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[-2].node));
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));      
}
#line 2390 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 57:
#line 703 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  ASTNode * n = GlobalParserInterface->newNode(BVSGT, *(yyvsp[-2].node), *(yyvsp[-1].node));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[-2].node));
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));      
}
#line 2401 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 58:
#line 710 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  ASTNode * n = GlobalParserInterface->newNode(BVSGE, *(yyvsp[-2].node), *(yyvsp[-1].node));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[-2].node));
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));      
}
#line 2412 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 59:
#line 717 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  ASTNode * n = GlobalParserInterface->newNode(BVLT, *(yyvsp[-2].node), *(yyvsp[-1].node));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[-2].node));
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));      
}
#line 2423 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 60:
#line 724 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  ASTNode * n = GlobalParserInterface->newNode(BVLE, *(yyvsp[-2].node), *(yyvsp[-1].node));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[-2].node));
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));      
}
#line 2434 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 61:
#line 731 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  ASTNode * n = GlobalParserInterface->newNode(BVGT, *(yyvsp[-2].node), *(yyvsp[-1].node));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[-2].node));
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));      
}
#line 2445 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 62:
#line 738 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  ASTNode * n = GlobalParserInterface->newNode(BVGE, *(yyvsp[-2].node), *(yyvsp[-1].node));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[-2].node));
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));      
}
#line 2456 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 63:
#line 745 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.node) = (yyvsp[-1].node);
}
#line 2464 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 64:
#line 749 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.node) = GlobalParserInterface->newNode(GlobalParserInterface->nf->CreateNode(NOT, *(yyvsp[-1].node)));
    GlobalParserInterface->deleteNode( (yyvsp[-1].node));
}
#line 2473 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 65:
#line 754 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.node) = GlobalParserInterface->newNode(IMPLIES, *(yyvsp[-2].node), *(yyvsp[-1].node));
  GlobalParserInterface->deleteNode( (yyvsp[-2].node));
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));      
}
#line 2483 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 66:
#line 760 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.node) = GlobalParserInterface->newNode(GlobalParserInterface->nf->CreateNode(ITE, *(yyvsp[-3].node), *(yyvsp[-2].node), *(yyvsp[-1].node)));
  GlobalParserInterface->deleteNode( (yyvsp[-3].node));
  GlobalParserInterface->deleteNode( (yyvsp[-2].node));      
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
}
#line 2494 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 67:
#line 767 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.node) = GlobalParserInterface->newNode(GlobalParserInterface->CreateNode(AND, *(yyvsp[-1].vec)));
  delete (yyvsp[-1].vec);
}
#line 2503 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 68:
#line 772 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.node) = GlobalParserInterface->newNode(GlobalParserInterface->CreateNode(OR, *(yyvsp[-1].vec)));
  delete (yyvsp[-1].vec);
}
#line 2512 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 69:
#line 777 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.node) = GlobalParserInterface->newNode(XOR, *(yyvsp[-2].node), *(yyvsp[-1].node));
  GlobalParserInterface->deleteNode( (yyvsp[-2].node));
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
}
#line 2522 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 70:
#line 783 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.node) = GlobalParserInterface->newNode(IFF, *(yyvsp[-2].node), *(yyvsp[-1].node));
  GlobalParserInterface->deleteNode( (yyvsp[-2].node));
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
}
#line 2532 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 71:
#line 789 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
    GlobalParserInterface->letMgr->push();  // TODO this isn't going to clear properly if it's an_term later.
  }
#line 2540 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 72:
#line 793 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
    (yyval.node) = (yyvsp[-1].node);
    GlobalParserInterface->letMgr->pop();
  }
#line 2549 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 73:
#line 798 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.node) = GlobalParserInterface->newNode(GlobalParserInterface->applyFunction(*(yyvsp[-2].str),*(yyvsp[-1].vec)));
  delete (yyvsp[-2].str);
  delete (yyvsp[-1].vec);
}
#line 2559 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 74:
#line 804 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  ASTVec empty;
  (yyval.node) = GlobalParserInterface->newNode(GlobalParserInterface->applyFunction(*(yyvsp[0].str),empty));
  delete (yyvsp[0].str);
}
#line 2569 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 75:
#line 810 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  /* 
    This implements (! <an_formula> :named foo) 
    "foo" is created as a symbol that can be refered to by later commands.
  */

  // TODO, will fail if name is already defined?
  ASTNode s(GlobalParserInterface->LookupOrCreateSymbol((yyvsp[-1].str)->c_str()));
  s.SetIndexWidth((yyvsp[-3].node)->GetIndexWidth());
  s.SetValueWidth((yyvsp[-3].node)->GetValueWidth());

  GlobalParserInterface->addSymbol(s);

  ASTNode n = GlobalParserInterface->CreateNode(IFF,s, *(yyvsp[-3].node));
 
  GlobalParserInterface->AddAssert(n);

  delete (yyvsp[-1].str);

  (yyval.node) = (yyvsp[-3].node);
}
#line 2595 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 77:
#line 835 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {}
#line 2601 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 78:
#line 838 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
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
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
}
#line 2619 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 79:
#line 852 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
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
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));

}
#line 2638 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 80:
#line 870 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.vec) = new ASTVec;
  if ((yyvsp[0].node) != NULL) {
    (yyval.vec)->push_back(*(yyvsp[0].node));
    GlobalParserInterface->deleteNode( (yyvsp[0].node));
  
  }
}
#line 2651 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 81:
#line 880 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  if ((yyvsp[-1].vec) != NULL && (yyvsp[0].node) != NULL) {
    (yyvsp[-1].vec)->push_back(*(yyvsp[0].node));
    (yyval.vec) = (yyvsp[-1].vec);
    GlobalParserInterface->deleteNode( (yyvsp[0].node));
  }
}
#line 2663 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 82:
#line 891 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.node) = GlobalParserInterface->newNode((*(yyvsp[0].node)));
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2672 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 83:
#line 896 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.node) = (yyvsp[-1].node);
}
#line 2680 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 84:
#line 900 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  //ARRAY READ
  // valuewidth is same as array, indexwidth is 0.
  ASTNode array = *(yyvsp[-1].node);
  ASTNode index = *(yyvsp[0].node);
  unsigned int width = array.GetValueWidth();
  ASTNode * n = 
    GlobalParserInterface->newNode(GlobalParserInterface->nf->CreateTerm(READ, width, array, index));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2697 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 85:
#line 913 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  //ARRAY WRITE
  unsigned int width = (yyvsp[0].node)->GetValueWidth();
  ASTNode array = *(yyvsp[-2].node);
  ASTNode index = *(yyvsp[-1].node);
  ASTNode writeval = *(yyvsp[0].node);
  ASTNode write_term = GlobalParserInterface->nf->CreateArrayTerm(WRITE,(yyvsp[-2].node)->GetIndexWidth(),width,array,index,writeval);
  ASTNode * n = GlobalParserInterface->newNode(write_term);
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[-2].node));
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2715 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 86:
#line 927 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  int width = (yyvsp[-3].uintval) - (yyvsp[-2].uintval) + 1;
  if (width < 0)
    yyerror("Negative width in extract");
      
  if((unsigned)(yyvsp[-3].uintval) >= (yyvsp[0].node)->GetValueWidth())
    yyerror("Parsing: Wrong width in BVEXTRACT\n");                      
      
  ASTNode hi  =  GlobalParserInterface->CreateBVConst(32, (yyvsp[-3].uintval));
  ASTNode low =  GlobalParserInterface->CreateBVConst(32, (yyvsp[-2].uintval));
  ASTNode output = GlobalParserInterface->nf->CreateTerm(BVEXTRACT, width, *(yyvsp[0].node),hi,low);
  ASTNode * n = GlobalParserInterface->newNode(output);
  (yyval.node) = n;
    GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2735 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 87:
#line 943 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  unsigned w = (yyvsp[0].node)->GetValueWidth() + (yyvsp[-2].uintval);
  ASTNode width = GlobalParserInterface->CreateBVConst(32,w);
  ASTNode *n =  GlobalParserInterface->newNode(GlobalParserInterface->nf->CreateTerm(BVZX,w,*(yyvsp[0].node),width));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2747 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 88:
#line 951 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  unsigned w = (yyvsp[0].node)->GetValueWidth() + (yyvsp[-2].uintval);
  ASTNode width = GlobalParserInterface->CreateBVConst(32,w);
  ASTNode *n =  GlobalParserInterface->newNode(GlobalParserInterface->nf->CreateTerm(BVSX,w,*(yyvsp[0].node),width));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2759 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 89:
#line 960 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  const unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  (yyval.node) = GlobalParserInterface->newNode(GlobalParserInterface->nf->CreateArrayTerm(ITE,(yyvsp[0].node)->GetIndexWidth(), width,*(yyvsp[-2].node), *(yyvsp[-1].node), *(yyvsp[0].node)));      
  GlobalParserInterface->deleteNode( (yyvsp[-2].node));
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2771 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 90:
#line 968 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  const unsigned int width = (yyvsp[-1].node)->GetValueWidth() + (yyvsp[0].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(GlobalParserInterface->nf->CreateTerm(BVCONCAT, width, *(yyvsp[-1].node), *(yyvsp[0].node)));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2783 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 91:
#line 976 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  //this is the BVNEG (term) in the CVCL language
  unsigned int width = (yyvsp[0].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(GlobalParserInterface->nf->CreateTerm(BVNOT, width, *(yyvsp[0].node)));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2795 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 92:
#line 984 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  //this is the BVUMINUS term in CVCL langauge
  unsigned width = (yyvsp[0].node)->GetValueWidth();
  ASTNode * n =  GlobalParserInterface->newNode(GlobalParserInterface->nf->CreateTerm(BVUMINUS,width,*(yyvsp[0].node)));
  (yyval.node) = n;
    GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2807 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 93:
#line 992 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(BVAND, width, *(yyvsp[-1].node), *(yyvsp[0].node));
  (yyval.node) = n;
    GlobalParserInterface->deleteNode( (yyvsp[-1].node));
    GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2819 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 94:
#line 1000 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(BVOR, width, *(yyvsp[-1].node), *(yyvsp[0].node)); 
  (yyval.node) = n;
    GlobalParserInterface->deleteNode( (yyvsp[-1].node));
    GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2831 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 95:
#line 1008 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(BVXOR, width, *(yyvsp[-1].node), *(yyvsp[0].node));
  (yyval.node) = n;
    GlobalParserInterface->deleteNode( (yyvsp[-1].node));
    GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2843 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 96:
#line 1016 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
//   (bvxnor s t) abbreviates (bvor (bvand s t) (bvand (bvnot s) (bvnot t)))
  unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(
  GlobalParserInterface->nf->CreateTerm( BVOR, width,
    GlobalParserInterface->nf->CreateTerm(BVAND, width, *(yyvsp[-1].node), *(yyvsp[0].node)),
      GlobalParserInterface->nf->CreateTerm(BVAND, width,
        GlobalParserInterface->nf->CreateTerm(BVNOT, width, *(yyvsp[-1].node)),
        GlobalParserInterface->nf->CreateTerm(BVNOT, width, *(yyvsp[0].node))
      )
    )
  );

  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2865 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 97:
#line 1034 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  ASTNode * n = GlobalParserInterface->newNode(GlobalParserInterface->nf->CreateTerm(ITE, 1,
  GlobalParserInterface->nf->CreateNode(EQ, *(yyvsp[-1].node), *(yyvsp[0].node)),
  GlobalParserInterface->CreateOneConst(1),
  GlobalParserInterface->CreateZeroConst(1)));

  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2880 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 98:
#line 1045 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  const unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(BVSUB, width, *(yyvsp[-1].node), *(yyvsp[0].node));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2892 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 99:
#line 1053 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  const unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(BVPLUS, width, *(yyvsp[-1].node), *(yyvsp[0].node));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
  GlobalParserInterface->deleteNode( (yyvsp[0].node));

}
#line 2905 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 100:
#line 1062 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  const unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(GlobalParserInterface->nf->CreateTerm(BVMULT, width, *(yyvsp[-1].node), *(yyvsp[0].node)));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2917 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 101:
#line 1070 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(BVDIV, width, *(yyvsp[-1].node), *(yyvsp[0].node));
  (yyval.node) = n;

  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2930 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 102:
#line 1079 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(BVMOD, width, *(yyvsp[-1].node), *(yyvsp[0].node));
  (yyval.node) = n;

  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2943 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 103:
#line 1088 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(SBVDIV, width, *(yyvsp[-1].node), *(yyvsp[0].node));
  (yyval.node) = n;

  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2956 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 104:
#line 1097 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(SBVREM, width, *(yyvsp[-1].node), *(yyvsp[0].node));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2968 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 105:
#line 1105 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(SBVMOD, width, *(yyvsp[-1].node), *(yyvsp[0].node));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2980 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 106:
#line 1113 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(GlobalParserInterface->nf->CreateTerm(BVNOT, width, GlobalParserInterface->nf->CreateTerm(BVAND, width, *(yyvsp[-1].node), *(yyvsp[0].node))));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 2992 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 107:
#line 1121 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(GlobalParserInterface->nf->CreateTerm(BVNOT, width, GlobalParserInterface->nf->CreateTerm(BVOR, width, *(yyvsp[-1].node), *(yyvsp[0].node)))); 
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 3004 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 108:
#line 1129 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  // shifting left by who know how much?
  unsigned int w = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(BVLEFTSHIFT,w,*(yyvsp[-1].node),*(yyvsp[0].node));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 3017 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 109:
#line 1138 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  // shifting right by who know how much?
  unsigned int w = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(BVRIGHTSHIFT,w,*(yyvsp[-1].node),*(yyvsp[0].node));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 3030 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 110:
#line 1147 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  // shifting arithmetic right by who know how much?
  unsigned int w = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n = GlobalParserInterface->newNode(BVSRSHIFT,w,*(yyvsp[-1].node),*(yyvsp[0].node));
  (yyval.node) = n;
  GlobalParserInterface->deleteNode( (yyvsp[-1].node));
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 3043 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 111:
#line 1156 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  ASTNode *n;
  unsigned width = (yyvsp[0].node)->GetValueWidth();
  unsigned rotate = (yyvsp[-2].uintval) % width;
  if (0 == rotate)
  {
      n = (yyvsp[0].node);
  }
  else 
  {
    ASTNode high = GlobalParserInterface->CreateBVConst(32,width-1);
    ASTNode zero = GlobalParserInterface->CreateBVConst(32,0);
    ASTNode cut = GlobalParserInterface->CreateBVConst(32,width-rotate);
    ASTNode cutMinusOne = GlobalParserInterface->CreateBVConst(32,width-rotate-1);

    ASTNode top =  GlobalParserInterface->nf->CreateTerm(BVEXTRACT,rotate,*(yyvsp[0].node),high, cut);
    ASTNode bottom =  GlobalParserInterface->nf->CreateTerm(BVEXTRACT,width-rotate,*(yyvsp[0].node),cutMinusOne,zero);
    n =  GlobalParserInterface->newNode(GlobalParserInterface->nf->CreateTerm(BVCONCAT,width,bottom,top));
    GlobalParserInterface->deleteNode( (yyvsp[0].node));
  }
  (yyval.node) = n;
}
#line 3070 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 112:
#line 1179 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  ASTNode *n;
  unsigned width = (yyvsp[0].node)->GetValueWidth();
  unsigned rotate = (yyvsp[-2].uintval) % width;
  if (0 == rotate)
  {
      n = (yyvsp[0].node);
  }
  else 
  {
    ASTNode high = GlobalParserInterface->CreateBVConst(32,width-1);
    ASTNode zero = GlobalParserInterface->CreateBVConst(32,0);
    ASTNode cut = GlobalParserInterface->CreateBVConst(32,rotate);
    ASTNode cutMinusOne = GlobalParserInterface->CreateBVConst(32,rotate-1);

    ASTNode bottom =  GlobalParserInterface->nf->CreateTerm(BVEXTRACT,rotate,*(yyvsp[0].node),cutMinusOne, zero);
    ASTNode top =  GlobalParserInterface->nf->CreateTerm(BVEXTRACT,width-rotate,*(yyvsp[0].node),high,cut);
    n =  GlobalParserInterface->newNode(GlobalParserInterface->nf->CreateTerm(BVCONCAT,width,bottom,top));
    GlobalParserInterface->deleteNode( (yyvsp[0].node));
  }
  (yyval.node) = n;
}
#line 3097 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 113:
#line 1202 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
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
  (yyval.node) = GlobalParserInterface->newNode(n);
  GlobalParserInterface->deleteNode( (yyvsp[0].node));
}
#line 3117 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 114:
#line 1218 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.node) = GlobalParserInterface->newNode(GlobalParserInterface->CreateBVConst(*(yyvsp[-1].str), 10, (yyvsp[0].uintval)));
  (yyval.node)->SetValueWidth((yyvsp[0].uintval));
  delete (yyvsp[-1].str);
}
#line 3127 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 115:
#line 1224 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  unsigned width = (yyvsp[0].str)->length()*4;
  (yyval.node) = GlobalParserInterface->newNode(GlobalParserInterface->CreateBVConst(*(yyvsp[0].str), 16, width));
  (yyval.node)->SetValueWidth(width);
  delete (yyvsp[0].str);
}
#line 3138 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 116:
#line 1231 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  unsigned width = (yyvsp[0].str)->length();
  (yyval.node) = GlobalParserInterface->newNode(GlobalParserInterface->CreateBVConst(*(yyvsp[0].str), 2, width));
  (yyval.node)->SetValueWidth(width);
  delete (yyvsp[0].str);
}
#line 3149 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 117:
#line 1238 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  (yyval.node) = GlobalParserInterface->newNode(GlobalParserInterface->applyFunction(*(yyvsp[-2].str),*(yyvsp[-1].vec)));

  if ((yyval.node)->GetType() != BITVECTOR_TYPE)
      yyerror("Must be bitvector type");

  delete (yyvsp[-2].str);
  delete (yyvsp[-1].vec);
}
#line 3163 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 118:
#line 1248 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  ASTVec empty;
  (yyval.node) = GlobalParserInterface->newNode(GlobalParserInterface->applyFunction(*(yyvsp[0].str),empty));

  if ((yyval.node)->GetType() != BITVECTOR_TYPE)
    yyerror("Must be bitvector type");

  delete (yyvsp[0].str);
}
#line 3177 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 119:
#line 1258 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
  /* This implements (! <an_term> :named foo) */

  ASTNode s(GlobalParserInterface->LookupOrCreateSymbol((yyvsp[-1].str)->c_str()));
  delete (yyvsp[-1].str);

  s.SetIndexWidth((yyvsp[-3].node)->GetIndexWidth());
  s.SetValueWidth((yyvsp[-3].node)->GetValueWidth());

  GlobalParserInterface->addSymbol(s);

  ASTNode n = GlobalParserInterface->CreateNode(EQ,s, *(yyvsp[-3].node));
 
  GlobalParserInterface->AddAssert(n);

  (yyval.node) = (yyvsp[-3].node);
}
#line 3199 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 120:
#line 1276 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
    GlobalParserInterface->letMgr->push();  
  }
#line 3207 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;

  case 121:
#line 1280 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1646  */
    {
    (yyval.node) = (yyvsp[-1].node);
    GlobalParserInterface->letMgr->pop();
  }
#line 3216 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
    break;


#line 3220 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.cpp" /* yacc.c:1646  */
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
      yyerror (YY_("syntax error"));
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
        yyerror (yymsgp);
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
                  yystos[yystate], yyvsp);
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
  yyerror (YY_("memory exhausted"));
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
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
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
  return yyresult;
}
#line 1287 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1906  */

