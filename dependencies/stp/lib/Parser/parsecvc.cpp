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
#define yyparse         cvcparse
#define yylex           cvclex
#define yyerror         cvcerror
#define yydebug         cvcdebug
#define yynerrs         cvcnerrs

#define yylval          cvclval
#define yychar          cvcchar

/* Copy the first part of user declarations.  */
#line 1 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:339  */

  /********************************************************************
   * AUTHORS: Vijay Ganesh
   *
   * BEGIN DATE: November, 2005
   *
  Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
  ********************************************************************/
  
#include "stp/Parser/parser.h"
#include "stp/cpp_interface.h"
#include "stp/Parser/LetMgr.h"

  using namespace stp;
  using std::cout;
  using std::cerr;
  using std::endl;
  
  // Suppress the bogus warning suppression in bison (it generates
  // compile error)
#undef __GNUC_MINOR__
  
#define YYLTYPE_IS_TRIVIAL 1
#define YYMAXDEPTH 1048576000
#define YYERROR_VERBOSE 1
#define YY_EXIT_FAILURE -1
  
  extern int cvclex(void);
  extern char* yytext;
  extern int cvclineno;
  int yyerror(const char *s) {
    cout << "syntax error: line " << cvclineno << "\n" << s << endl;    
    FatalError("");
    return YY_EXIT_FAILURE;
  }
  int yyerror(void* /*AssertsQuery*/, const char* s) { return yyerror(s); }
  
  

#line 130 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:339  */

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
   by #include "parsecvc.hpp".  */
#ifndef YY_CVC_HOME_MAP2CHECK_DEVEL_TOOL_MAP_SRC_ON_DOCKER_DEPENDENCIES_STP_LIB_PARSER_PARSECVC_HPP_INCLUDED
# define YY_CVC_HOME_MAP2CHECK_DEVEL_TOOL_MAP_SRC_ON_DOCKER_DEPENDENCIES_STP_LIB_PARSER_PARSECVC_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int cvcdebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    AND_TOK = 258,
    OR_TOK = 259,
    NOT_TOK = 260,
    EXCEPT_TOK = 261,
    XOR_TOK = 262,
    NAND_TOK = 263,
    NOR_TOK = 264,
    IMPLIES_TOK = 265,
    IFF_TOK = 266,
    IF_TOK = 267,
    THEN_TOK = 268,
    ELSE_TOK = 269,
    ELSIF_TOK = 270,
    END_TOK = 271,
    ENDIF_TOK = 272,
    NEQ_TOK = 273,
    ASSIGN_TOK = 274,
    BV_TOK = 275,
    BVLEFTSHIFT_TOK = 276,
    BVRIGHTSHIFT_TOK = 277,
    BVSHL_TOK = 278,
    BVLSHR_TOK = 279,
    BVASHR_TOK = 280,
    BVPLUS_TOK = 281,
    BVSUB_TOK = 282,
    BVUMINUS_TOK = 283,
    BVMULT_TOK = 284,
    BVDIV_TOK = 285,
    BVMOD_TOK = 286,
    SBVDIV_TOK = 287,
    SBVREM_TOK = 288,
    BVNEG_TOK = 289,
    BVAND_TOK = 290,
    BVOR_TOK = 291,
    BVXOR_TOK = 292,
    BVNAND_TOK = 293,
    BVNOR_TOK = 294,
    BVXNOR_TOK = 295,
    BVCONCAT_TOK = 296,
    BVLT_TOK = 297,
    BVGT_TOK = 298,
    BVLE_TOK = 299,
    BVGE_TOK = 300,
    BVSLT_TOK = 301,
    BVSGT_TOK = 302,
    BVSLE_TOK = 303,
    BVSGE_TOK = 304,
    BOOL_TO_BV_TOK = 305,
    BVSX_TOK = 306,
    BVZX_TOK = 307,
    BOOLEXTRACT_TOK = 308,
    ASSERT_TOK = 309,
    QUERY_TOK = 310,
    BOOLEAN_TOK = 311,
    ARRAY_TOK = 312,
    OF_TOK = 313,
    WITH_TOK = 314,
    TRUELIT_TOK = 315,
    FALSELIT_TOK = 316,
    IN_TOK = 317,
    LET_TOK = 318,
    PUSH_TOK = 319,
    POP_TOK = 320,
    BVCONST_TOK = 321,
    TERMID_TOK = 322,
    FORMID_TOK = 323,
    COUNTEREXAMPLE_TOK = 324,
    NUMERAL_TOK = 325,
    BIN_BASED_NUMBER = 326,
    DEC_BASED_NUMBER = 327,
    HEX_BASED_NUMBER = 328,
    STRING_TOK = 329
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 58 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:355  */


  unsigned int uintval;                 /* for numerals in types. */
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
  vector<char*> * stringVec;
  char* str;

  //Hash_Map to hold Array Updates during parse A map from array index
  //to array values. To support the WITH construct
  stp::ASTNodeMap * Index_To_UpdateValue;

#line 268 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE cvclval;

int cvcparse (void* AssertsQuery);

#endif /* !YY_CVC_HOME_MAP2CHECK_DEVEL_TOOL_MAP_SRC_ON_DOCKER_DEPENDENCIES_STP_LIB_PARSER_PARSECVC_HPP_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 285 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:358  */

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
#define YYFINAL  60
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1515

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  85
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  27
/* YYNRULES -- Number of rules.  */
#define YYNRULES  112
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  347

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   329

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
      70,    83,     2,     2,    82,     2,    69,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    81,    80,
       2,    66,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    67,     2,    84,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    68,     2,     2,     2,     2,     2,     2,
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
      65,    71,    72,    73,    74,    75,    76,    77,    78,    79
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   205,   205,   209,   215,   222,   237,   243,   249,   267,
     271,   275,   295,   302,   311,   317,   322,   325,   330,   341,
     356,   373,   379,   387,   403,   404,   405,   408,   420,   426,
     434,   452,   453,   472,   476,   480,   486,   498,   505,   512,
     517,   523,   529,   535,   541,   547,   553,   559,   566,   573,
     580,   587,   594,   601,   608,   615,   616,   622,   629,   638,
     647,   648,   654,   662,   669,   679,   680,   681,   682,   692,
     698,   704,   710,   720,   730,   745,   752,   763,   774,   785,
     797,   809,   821,   833,   845,   854,   870,   886,   906,   922,
     931,   940,   949,   956,   964,   971,   979,   987,   995,  1003,
    1010,  1011,  1012,  1015,  1023,  1053,  1060,  1069,  1070,  1073,
    1092,  1106,  1116
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "\"AND\"", "\"OR\"", "\"NOT\"",
  "\"EXCEPT\"", "\"XOR\"", "\"NAND\"", "\"NOR\"", "\"=>\"", "\"<=>\"",
  "\"IF\"", "\"THEN\"", "\"ELSE\"", "\"ELSIF\"", "\"END\"", "\"ENDIF\"",
  "\"/=\"", "\":=\"", "\"BV\"", "\"<<\"", "\">>\"", "\"BVSHL\"",
  "\"BVLSHR\"", "\"BVASHR\"", "\"BVPLUS\"", "\"BVSUB\"", "\"BVUMINUS\"",
  "\"BVMULT\"", "\"BVDIV\"", "\"BVMOD\"", "\"SBVDIV\"", "\"SBVREM\"",
  "\"~\"", "\"&\"", "\"|\"", "\"BVXOR\"", "\"BVNAND\"", "\"BVNOR\"",
  "\"BVXNOR\"", "\"@\"", "\"BVLT\"", "\"BVGT\"", "\"BVLE\"", "\"BVGE\"",
  "\"BVSLT\"", "\"BVSGT\"", "\"BVSLE\"", "\"BVSGE\"", "\"BOOLBV\"",
  "\"BVSX\"", "\"BVZX\"", "\"BOOLEXTRACT\"", "\"ASSERT\"", "\"QUERY\"",
  "\"BOOLEAN\"", "\"ARRAY\"", "\"OF\"", "\"WITH\"", "\"TRUE\"",
  "\"FALSE\"", "\"IN\"", "\"LET\"", "\"PUSH\"", "\"POP\"", "'='", "'['",
  "'{'", "'.'", "'('", "BVCONST_TOK", "TERMID_TOK", "FORMID_TOK",
  "COUNTEREXAMPLE_TOK", "NUMERAL_TOK", "BIN_BASED_NUMBER",
  "DEC_BASED_NUMBER", "HEX_BASED_NUMBER", "STRING_TOK", "';'", "':'",
  "','", "')'", "']'", "$accept", "cmd", "counterexample", "other_cmd",
  "other_cmd1", "Asserts", "Assert", "Query", "VarDecls", "VarDecl",
  "reverseFORM_IDs", "FORM_IDs", "Type", "BvType", "BoolType", "ArrayType",
  "IfExpr", "ElseRestExpr", "Formula", "IfForm", "ElseRestForm", "Exprs",
  "Expr", "ArrayUpdateExpr", "Updates", "LetDecls", "LetDecl", YY_NULLPTR
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
     315,   316,   317,   318,   319,   320,    61,    91,   123,    46,
      40,   321,   322,   323,   324,   325,   326,   327,   328,   329,
      59,    58,    44,    41,    93
};
# endif

#define YYPACT_NINF -130

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-130)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -48,   378,   378,   -74,    76,    -7,   -33,    30,  -130,  -130,
     -48,    12,  -130,   -11,   378,   378,    26,    33,    34,    55,
      66,    69,    72,    80,    87,    89,    93,   435,    98,   101,
     103,   106,   127,   131,   137,   148,   156,   157,   159,   161,
     162,   165,   166,   169,  -130,  -130,    61,   378,  -130,  -130,
     170,    84,  -130,  -130,    25,  -130,  1424,  -130,    40,    75,
    -130,    15,  -130,  -130,   -24,  -130,    30,  -130,   163,  -130,
     -15,  -130,   486,   435,   435,   435,   167,   172,   435,   178,
     180,   183,   193,   194,   378,    61,   435,   174,   435,   435,
     435,   435,   435,   435,   435,   435,   435,   435,   435,   435,
     378,   435,   435,   435,     0,   -10,  -130,     7,    65,   435,
    -130,  -130,  -130,   378,   378,   378,   378,   378,   378,   378,
    -130,   435,   492,   549,   435,   435,   435,   197,   435,   606,
     435,  -130,  -130,  -130,    30,  -130,   175,  -130,   240,   205,
    -130,  -130,  -130,   378,   953,   963,   996,   200,   203,   108,
     206,   207,   208,   209,   210,   543,     9,   187,  1003,  1005,
    1047,  1068,  1072,  1089,  1111,  1139,  1141,  1166,  1183,  1193,
      16,  1210,  1235,  1237,   378,   -15,   378,    61,  -130,  -130,
     189,    -5,   143,   195,   267,  -130,    70,    70,  1445,    84,
      67,    84,    67,   174,    88,   216,   435,  -130,  1445,   185,
      18,   304,   218,   236,   378,    54,  1403,   435,   435,   435,
     435,   435,  -130,   435,   435,   435,   435,   435,   435,   435,
     435,   435,   435,   435,   435,   435,   435,   435,   435,   435,
     435,   435,  -130,   221,   222,   228,   270,  1424,   257,   270,
    1424,  -130,  -130,    86,   252,  -130,  -130,   245,   240,   270,
    1424,   378,   378,  -130,  -130,   435,   435,  -130,   314,   627,
     629,   -45,  1445,  1262,  1279,  1289,  1306,  1331,  1333,  1412,
    1445,   631,   670,   682,   703,   710,   746,   753,   763,   796,
     803,   813,   846,   246,   247,   248,   378,   313,   249,  -130,
    -130,   349,   600,  1361,  1373,  -130,  -130,  -130,   435,  -130,
     435,   435,   435,   435,   435,   435,  -130,  -130,  -130,  -130,
    -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,
    -130,   270,  1424,   435,  -130,  -130,   378,  -130,   435,  1445,
     853,   863,   896,   903,   913,   946,    27,    54,  1412,  -130,
    -130,  -130,  -130,  -130,  -130,  -130,  -130
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       5,     0,     0,    21,     0,     2,     0,    10,    12,     6,
       0,     0,    23,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    56,    57,     0,     0,    67,    65,
      34,     0,   103,   100,     0,    55,     0,   101,     0,     0,
       1,     0,     3,     8,     0,    13,     9,     7,     0,    16,
       0,    39,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    75,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   107,     0,     0,     0,
      69,    70,    71,     0,     0,     0,     0,     0,     0,     0,
      14,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    15,    22,     4,    11,    17,     0,    28,     0,    18,
      24,    25,    26,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    33,    66,
       0,    42,    40,    46,    43,    41,    44,    45,    38,    85,
      86,    87,    88,    76,    77,    84,     0,   104,    37,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    94,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    68,     0,     0,     0,   111,   109,     0,    58,
     102,   108,    35,     0,     0,    72,    73,     0,     0,    20,
      19,     0,     0,    62,    59,     0,     0,    30,     0,     0,
       0,     0,    63,     0,     0,     0,     0,     0,     0,     0,
     102,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    27,
      29,     0,     0,     0,     0,    89,    90,    91,     0,    92,
       0,     0,     0,     0,     0,     0,    78,    79,    80,    81,
      47,    48,    49,    50,    51,    52,    53,    54,    82,    83,
      36,   112,   110,     0,    74,    60,     0,    31,     0,    64,
       0,     0,     0,     0,     0,     0,   105,     0,     0,    93,
      95,    96,    97,    98,    99,    61,    32
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -130,  -130,  -130,  -130,  -130,    -8,    -6,    83,   328,    11,
     278,  -130,   168,  -129,  -130,  -130,  -130,     3,    -2,  -130,
       1,  -130,    91,  -130,  -130,   259,   171
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     4,    62,     5,     6,     7,     8,     9,    10,    11,
      12,    13,   139,   140,   141,   142,    53,   257,    54,    55,
     254,   261,    56,    57,   197,   105,   106
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint16 yytable[] =
{
      58,    65,    66,   116,   117,   136,     1,     2,    59,   203,
     113,   114,    71,    72,   115,   116,   117,   118,   119,   113,
     114,    68,     2,   115,   116,   117,   118,   119,   113,   114,
       1,     3,   115,   116,   117,   118,   119,   298,   299,   122,
     123,   137,   138,   113,   114,   107,     3,   115,   116,   117,
     118,   119,   176,   124,   125,     3,   134,   113,   114,   126,
      65,   115,   116,   117,   118,   119,   174,    61,   251,   252,
      70,   219,   177,   113,   114,    68,    60,   127,   116,   117,
     118,   175,   155,   121,     1,   129,   122,   123,   130,    63,
     178,   177,    69,    67,   129,   133,    73,   130,   170,   232,
     124,   125,   245,    74,    75,   120,   126,   122,   123,   122,
     123,   181,   182,   183,   184,   185,   186,   187,    87,   290,
     131,   124,   125,   124,   127,    76,   127,   126,    65,   122,
     123,   128,   129,   253,   129,   130,    77,   130,   108,    78,
     104,   205,    79,   124,   125,   127,   113,   127,   179,   126,
      80,   116,   117,   129,     3,   129,   130,    81,   130,    82,
     110,   111,   112,    83,   144,   145,   146,   127,    88,   149,
     287,    89,   236,    90,   239,   129,    91,   157,   130,   158,
     159,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     169,   212,   171,   172,   173,   122,   123,    92,   113,   114,
     180,    93,   249,   116,   117,   118,   119,    94,   122,   123,
     122,   123,   188,   190,   192,   193,   194,   195,    95,   198,
     200,   201,   124,   125,   124,   125,    96,    97,   126,    98,
     126,    99,   100,   127,   206,   101,   102,   122,   123,   103,
     109,   129,   147,   135,   130,   202,   127,   148,   127,   291,
     292,   124,   125,   150,   129,   151,   129,   130,   152,   130,
     136,   110,   111,   112,   196,   237,   244,   240,   153,   154,
     179,   204,   242,   113,   114,   127,   117,   115,   116,   117,
     118,   119,   210,   129,   321,   211,   130,   243,   213,   214,
     215,   216,   217,   247,   248,   250,   283,   284,   258,   259,
     260,   262,   263,   285,   264,   265,   266,   267,   268,   269,
     270,   271,   272,   273,   274,   275,   276,   277,   278,   279,
     280,   281,   282,   286,   337,   122,   123,   288,   289,   318,
     319,   320,   323,   324,    64,   122,   123,   132,   345,   124,
     125,   346,     0,   238,   156,   126,   293,   294,   241,   124,
     125,     0,   113,   114,     0,   126,   115,   116,   117,   118,
     119,     0,     0,   127,     0,     0,   325,     0,     0,     0,
       0,   129,     0,   127,   130,     0,     0,   322,     0,     0,
       0,   129,     0,    14,   130,     0,     0,   246,     0,   329,
      15,   330,   331,   332,   333,   334,   335,   295,     0,     0,
       0,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,     0,   336,    28,    29,    30,    31,   338,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,     0,     0,     0,     0,     0,     0,    44,    45,
       0,    46,     0,     0,     0,     0,     0,    84,    47,    48,
      49,    50,     0,    51,     0,     0,     0,    52,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
       0,     0,    28,    29,    30,    31,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    40,    41,    42,     0,   113,
     114,     0,     0,   115,   116,   117,   118,   119,    85,   143,
       0,     0,     0,     0,    84,    86,    48,    49,     0,     0,
      51,     0,     0,     0,    52,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,     0,     0,    28,
      29,    30,    31,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    40,    41,    42,     0,   113,   114,     0,     0,
     115,   116,   117,   118,   119,    85,   218,     0,     0,     0,
       0,    84,    86,    48,    49,     0,     0,   189,     0,     0,
       0,    52,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,     0,     0,    28,    29,    30,    31,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    40,
      41,    42,     0,   113,   114,     0,     0,   115,   116,   117,
     118,   119,    85,   326,     0,     0,     0,     0,    84,    86,
      48,    49,     0,     0,   191,     0,     0,     0,    52,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,     0,     0,    28,    29,    30,    31,     0,   122,   123,
     122,   123,   122,   123,     0,     0,    40,    41,    42,     0,
       0,     0,   124,   125,   124,   125,   124,   125,   126,    85,
     126,     0,   126,     0,     0,     0,    86,    48,    49,     0,
       0,   199,     0,     0,     0,    52,   127,     0,   127,     0,
     127,   122,   123,     0,   129,     0,   129,   130,   129,   130,
       0,   130,     0,   122,   123,   124,   125,     0,     0,     0,
     296,   126,   297,     0,   306,     0,     0,   124,   125,     0,
       0,     0,     0,   126,   122,   123,     0,     0,     0,   127,
       0,   122,   123,     0,     0,     0,     0,   129,   124,   125,
     130,   127,     0,     0,   126,   124,   125,     0,     0,   129,
       0,   126,   130,   307,     0,     0,     0,     0,     0,     0,
       0,     0,   127,     0,     0,   308,     0,   122,   123,   127,
     129,     0,     0,   130,   122,   123,     0,   129,     0,     0,
     130,   124,   125,     0,   122,   123,   309,   126,   124,   125,
       0,     0,     0,   310,   126,     0,     0,     0,   124,   125,
       0,     0,     0,     0,   126,   127,     0,     0,     0,     0,
       0,     0,   127,   129,     0,     0,   130,   122,   123,     0,
     129,     0,   127,   130,   122,   123,     0,     0,     0,   311,
     129,   124,   125,   130,   122,   123,   312,   126,   124,   125,
       0,     0,     0,     0,   126,     0,   313,     0,   124,   125,
       0,     0,     0,     0,   126,   127,     0,     0,     0,     0,
       0,     0,   127,   129,     0,     0,   130,   122,   123,     0,
     129,     0,   127,   130,   122,   123,     0,     0,     0,   314,
     129,   124,   125,   130,   122,   123,   315,   126,   124,   125,
       0,     0,     0,     0,   126,     0,   316,     0,   124,   125,
       0,     0,     0,     0,   126,   127,     0,     0,     0,     0,
       0,     0,   127,   129,     0,     0,   130,   122,   123,     0,
     129,     0,   127,   130,   122,   123,     0,     0,     0,   317,
     129,   124,   125,   130,   122,   123,   339,   126,   124,   125,
       0,     0,     0,     0,   126,     0,   340,     0,   124,   125,
       0,     0,     0,     0,   126,   127,     0,     0,     0,     0,
       0,     0,   127,   129,     0,     0,   130,   122,   123,     0,
     129,     0,   127,   130,   122,   123,     0,     0,     0,   341,
     129,   124,   125,   130,   122,   123,   342,   126,   124,   125,
       0,     0,     0,     0,   126,     0,   343,     0,   124,   125,
       0,     0,     0,     0,   126,   127,     0,     0,     0,     0,
       0,     0,   127,   129,     0,     0,   130,   122,   123,     0,
     129,     0,   127,   130,   122,   123,   122,   123,     0,   344,
     129,   124,   125,   130,     0,   207,     0,   126,   124,   125,
     124,   125,     0,     0,   126,   208,   126,     0,     0,     0,
       0,     0,     0,     0,     0,   127,     0,     0,     0,     0,
       0,     0,   127,   129,   127,     0,   130,     0,   122,   123,
     129,     0,   129,   130,     0,   130,     0,     0,   209,     0,
       0,     0,   124,   125,     0,   220,     0,   221,   126,   122,
     123,     0,     0,   122,   123,     0,     0,     0,     0,     0,
       0,     0,     0,   124,   125,     0,   127,   124,   125,   126,
     122,   123,     0,   126,   129,     0,     0,   130,     0,     0,
       0,     0,     0,     0,   124,   125,     0,   127,     0,   222,
     126,   127,   122,   123,     0,   129,     0,     0,   130,   129,
       0,     0,   130,     0,     0,     0,   124,   125,   127,     0,
     223,     0,   126,     0,   224,     0,   129,     0,     0,   130,
     122,   123,   122,   123,     0,     0,     0,     0,     0,     0,
     127,   225,     0,     0,   124,   125,   124,   125,   129,     0,
     126,   130,   126,     0,     0,     0,     0,   122,   123,     0,
       0,     0,     0,   226,     0,     0,     0,     0,   127,     0,
     127,   124,   125,     0,   122,   123,   129,   126,   129,   130,
       0,   130,     0,     0,   122,   123,     0,     0,   124,   125,
       0,   227,     0,   228,   126,   127,     0,     0,   124,   125,
       0,   122,   123,   129,   126,     0,   130,     0,     0,     0,
       0,     0,   127,     0,     0,   124,   125,     0,   229,     0,
     129,   126,   127,   130,     0,     0,   122,   123,   122,   123,
     129,     0,     0,   130,     0,   230,     0,     0,     0,   127,
     124,   125,   124,   125,     0,   231,   126,   129,   126,     0,
     130,     0,     0,   122,   123,     0,     0,     0,     0,     0,
       0,     0,   233,     0,   127,     0,   127,   124,   125,     0,
     122,   123,   129,   126,   129,   130,     0,   130,     0,     0,
     122,   123,     0,     0,   124,   125,     0,   234,     0,   235,
     126,   127,     0,     0,   124,   125,     0,   122,   123,   129,
     126,     0,   130,     0,     0,     0,     0,     0,   127,     0,
       0,   124,   125,     0,   300,     0,   129,   126,   127,   130,
       0,     0,   122,   123,   122,   123,   129,     0,     0,   130,
       0,   301,     0,     0,     0,   127,   124,   125,   124,   125,
       0,   302,   126,   129,   126,     0,   130,     0,   327,     0,
       0,     0,   122,   123,     0,     0,   328,     0,   303,     0,
     127,     0,   127,     0,   122,   123,   124,   125,   129,     0,
     129,   130,   126,   130,     0,     0,     0,     0,   124,   125,
       0,     0,     0,   304,   126,   305,     0,   255,   256,     0,
     127,   121,     0,     0,   122,   123,   255,   256,   129,     0,
       0,   130,   127,   122,   123,     0,     0,     0,   124,   125,
     129,     0,   121,   130,   126,   122,   123,   124,   125,     0,
       0,     0,     0,   126,     0,     0,     0,     0,     0,   124,
     125,     0,   127,     0,     0,   126,   122,   123,     0,   128,
     129,   127,     0,   130,     0,     0,     0,     0,     0,   129,
     124,   125,   130,   127,     0,     0,   126,     0,     0,     0,
     128,   129,     0,     0,   130,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   127,     0,     0,     0,     0,     0,
       0,     0,   129,     0,     0,   130
};

static const yytype_int16 yycheck[] =
{
       2,     7,    10,     8,     9,    20,    54,    55,    82,   138,
       3,     4,    14,    15,     7,     8,     9,    10,    11,     3,
       4,    10,    55,     7,     8,     9,    10,    11,     3,     4,
      54,    79,     7,     8,     9,    10,    11,    82,    83,    21,
      22,    56,    57,     3,     4,    47,    79,     7,     8,     9,
      10,    11,    62,    35,    36,    79,    64,     3,     4,    41,
      66,     7,     8,     9,    10,    11,    66,    74,    14,    15,
      81,    62,    82,     3,     4,    64,     0,    59,     8,     9,
      10,    81,    84,    18,    54,    67,    21,    22,    70,     6,
      83,    82,    80,    10,    67,    80,    70,    70,   100,    83,
      35,    36,    84,    70,    70,    80,    41,    21,    22,    21,
      22,   113,   114,   115,   116,   117,   118,   119,    27,   248,
      80,    35,    36,    35,    59,    70,    59,    41,   134,    21,
      22,    66,    67,    79,    67,    70,    70,    70,    47,    70,
      79,   143,    70,    35,    36,    59,     3,    59,    83,    41,
      70,     8,     9,    67,    79,    67,    70,    70,    70,    70,
      76,    77,    78,    70,    73,    74,    75,    59,    70,    78,
      84,    70,   174,    70,   176,    67,    70,    86,    70,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,    83,   101,   102,   103,    21,    22,    70,     3,     4,
     109,    70,   204,     8,     9,    10,    11,    70,    21,    22,
      21,    22,   121,   122,   123,   124,   125,   126,    70,   128,
     129,   130,    35,    36,    35,    36,    70,    70,    41,    70,
      41,    70,    70,    59,   143,    70,    70,    21,    22,    70,
      70,    67,    75,    80,    70,    70,    59,    75,    59,   251,
     252,    35,    36,    75,    67,    75,    67,    70,    75,    70,
      20,    76,    77,    78,    67,   174,    81,   176,    75,    75,
      83,    66,    83,     3,     4,    59,     9,     7,     8,     9,
      10,    11,    82,    67,   286,    82,    70,   196,    82,    82,
      82,    82,    82,    75,    58,   204,    75,    75,   207,   208,
     209,   210,   211,    75,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,    66,   326,    21,    22,    75,    83,    83,
      83,    83,    19,    84,     6,    21,    22,    59,   337,    35,
      36,   338,    -1,   175,    85,    41,   255,   256,   177,    35,
      36,    -1,     3,     4,    -1,    41,     7,     8,     9,    10,
      11,    -1,    -1,    59,    -1,    -1,    17,    -1,    -1,    -1,
      -1,    67,    -1,    59,    70,    -1,    -1,   286,    -1,    -1,
      -1,    67,    -1,     5,    70,    -1,    -1,    83,    -1,   298,
      12,   300,   301,   302,   303,   304,   305,    83,    -1,    -1,
      -1,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    -1,   323,    37,    38,    39,    40,   328,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    -1,    -1,    -1,    -1,    -1,    -1,    60,    61,
      -1,    63,    -1,    -1,    -1,    -1,    -1,    12,    70,    71,
      72,    73,    -1,    75,    -1,    -1,    -1,    79,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      -1,    -1,    37,    38,    39,    40,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    50,    51,    52,    -1,     3,
       4,    -1,    -1,     7,     8,     9,    10,    11,    63,    13,
      -1,    -1,    -1,    -1,    12,    70,    71,    72,    -1,    -1,
      75,    -1,    -1,    -1,    79,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    -1,    -1,    37,
      38,    39,    40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    50,    51,    52,    -1,     3,     4,    -1,    -1,
       7,     8,     9,    10,    11,    63,    13,    -1,    -1,    -1,
      -1,    12,    70,    71,    72,    -1,    -1,    75,    -1,    -1,
      -1,    79,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    -1,    -1,    37,    38,    39,    40,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,
      51,    52,    -1,     3,     4,    -1,    -1,     7,     8,     9,
      10,    11,    63,    13,    -1,    -1,    -1,    -1,    12,    70,
      71,    72,    -1,    -1,    75,    -1,    -1,    -1,    79,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    -1,    -1,    37,    38,    39,    40,    -1,    21,    22,
      21,    22,    21,    22,    -1,    -1,    50,    51,    52,    -1,
      -1,    -1,    35,    36,    35,    36,    35,    36,    41,    63,
      41,    -1,    41,    -1,    -1,    -1,    70,    71,    72,    -1,
      -1,    75,    -1,    -1,    -1,    79,    59,    -1,    59,    -1,
      59,    21,    22,    -1,    67,    -1,    67,    70,    67,    70,
      -1,    70,    -1,    21,    22,    35,    36,    -1,    -1,    -1,
      83,    41,    83,    -1,    83,    -1,    -1,    35,    36,    -1,
      -1,    -1,    -1,    41,    21,    22,    -1,    -1,    -1,    59,
      -1,    21,    22,    -1,    -1,    -1,    -1,    67,    35,    36,
      70,    59,    -1,    -1,    41,    35,    36,    -1,    -1,    67,
      -1,    41,    70,    83,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    59,    -1,    -1,    83,    -1,    21,    22,    59,
      67,    -1,    -1,    70,    21,    22,    -1,    67,    -1,    -1,
      70,    35,    36,    -1,    21,    22,    83,    41,    35,    36,
      -1,    -1,    -1,    83,    41,    -1,    -1,    -1,    35,    36,
      -1,    -1,    -1,    -1,    41,    59,    -1,    -1,    -1,    -1,
      -1,    -1,    59,    67,    -1,    -1,    70,    21,    22,    -1,
      67,    -1,    59,    70,    21,    22,    -1,    -1,    -1,    83,
      67,    35,    36,    70,    21,    22,    83,    41,    35,    36,
      -1,    -1,    -1,    -1,    41,    -1,    83,    -1,    35,    36,
      -1,    -1,    -1,    -1,    41,    59,    -1,    -1,    -1,    -1,
      -1,    -1,    59,    67,    -1,    -1,    70,    21,    22,    -1,
      67,    -1,    59,    70,    21,    22,    -1,    -1,    -1,    83,
      67,    35,    36,    70,    21,    22,    83,    41,    35,    36,
      -1,    -1,    -1,    -1,    41,    -1,    83,    -1,    35,    36,
      -1,    -1,    -1,    -1,    41,    59,    -1,    -1,    -1,    -1,
      -1,    -1,    59,    67,    -1,    -1,    70,    21,    22,    -1,
      67,    -1,    59,    70,    21,    22,    -1,    -1,    -1,    83,
      67,    35,    36,    70,    21,    22,    83,    41,    35,    36,
      -1,    -1,    -1,    -1,    41,    -1,    83,    -1,    35,    36,
      -1,    -1,    -1,    -1,    41,    59,    -1,    -1,    -1,    -1,
      -1,    -1,    59,    67,    -1,    -1,    70,    21,    22,    -1,
      67,    -1,    59,    70,    21,    22,    -1,    -1,    -1,    83,
      67,    35,    36,    70,    21,    22,    83,    41,    35,    36,
      -1,    -1,    -1,    -1,    41,    -1,    83,    -1,    35,    36,
      -1,    -1,    -1,    -1,    41,    59,    -1,    -1,    -1,    -1,
      -1,    -1,    59,    67,    -1,    -1,    70,    21,    22,    -1,
      67,    -1,    59,    70,    21,    22,    21,    22,    -1,    83,
      67,    35,    36,    70,    -1,    82,    -1,    41,    35,    36,
      35,    36,    -1,    -1,    41,    82,    41,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    59,    -1,    -1,    -1,    -1,
      -1,    -1,    59,    67,    59,    -1,    70,    -1,    21,    22,
      67,    -1,    67,    70,    -1,    70,    -1,    -1,    82,    -1,
      -1,    -1,    35,    36,    -1,    82,    -1,    82,    41,    21,
      22,    -1,    -1,    21,    22,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    35,    36,    -1,    59,    35,    36,    41,
      21,    22,    -1,    41,    67,    -1,    -1,    70,    -1,    -1,
      -1,    -1,    -1,    -1,    35,    36,    -1,    59,    -1,    82,
      41,    59,    21,    22,    -1,    67,    -1,    -1,    70,    67,
      -1,    -1,    70,    -1,    -1,    -1,    35,    36,    59,    -1,
      82,    -1,    41,    -1,    82,    -1,    67,    -1,    -1,    70,
      21,    22,    21,    22,    -1,    -1,    -1,    -1,    -1,    -1,
      59,    82,    -1,    -1,    35,    36,    35,    36,    67,    -1,
      41,    70,    41,    -1,    -1,    -1,    -1,    21,    22,    -1,
      -1,    -1,    -1,    82,    -1,    -1,    -1,    -1,    59,    -1,
      59,    35,    36,    -1,    21,    22,    67,    41,    67,    70,
      -1,    70,    -1,    -1,    21,    22,    -1,    -1,    35,    36,
      -1,    82,    -1,    82,    41,    59,    -1,    -1,    35,    36,
      -1,    21,    22,    67,    41,    -1,    70,    -1,    -1,    -1,
      -1,    -1,    59,    -1,    -1,    35,    36,    -1,    82,    -1,
      67,    41,    59,    70,    -1,    -1,    21,    22,    21,    22,
      67,    -1,    -1,    70,    -1,    82,    -1,    -1,    -1,    59,
      35,    36,    35,    36,    -1,    82,    41,    67,    41,    -1,
      70,    -1,    -1,    21,    22,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    82,    -1,    59,    -1,    59,    35,    36,    -1,
      21,    22,    67,    41,    67,    70,    -1,    70,    -1,    -1,
      21,    22,    -1,    -1,    35,    36,    -1,    82,    -1,    82,
      41,    59,    -1,    -1,    35,    36,    -1,    21,    22,    67,
      41,    -1,    70,    -1,    -1,    -1,    -1,    -1,    59,    -1,
      -1,    35,    36,    -1,    82,    -1,    67,    41,    59,    70,
      -1,    -1,    21,    22,    21,    22,    67,    -1,    -1,    70,
      -1,    82,    -1,    -1,    -1,    59,    35,    36,    35,    36,
      -1,    82,    41,    67,    41,    -1,    70,    -1,    17,    -1,
      -1,    -1,    21,    22,    -1,    -1,    13,    -1,    82,    -1,
      59,    -1,    59,    -1,    21,    22,    35,    36,    67,    -1,
      67,    70,    41,    70,    -1,    -1,    -1,    -1,    35,    36,
      -1,    -1,    -1,    82,    41,    82,    -1,    14,    15,    -1,
      59,    18,    -1,    -1,    21,    22,    14,    15,    67,    -1,
      -1,    70,    59,    21,    22,    -1,    -1,    -1,    35,    36,
      67,    -1,    18,    70,    41,    21,    22,    35,    36,    -1,
      -1,    -1,    -1,    41,    -1,    -1,    -1,    -1,    -1,    35,
      36,    -1,    59,    -1,    -1,    41,    21,    22,    -1,    66,
      67,    59,    -1,    70,    -1,    -1,    -1,    -1,    -1,    67,
      35,    36,    70,    59,    -1,    -1,    41,    -1,    -1,    -1,
      66,    67,    -1,    -1,    70,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    59,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    67,    -1,    -1,    70
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    54,    55,    79,    86,    88,    89,    90,    91,    92,
      93,    94,    95,    96,     5,    12,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    37,    38,
      39,    40,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    60,    61,    63,    70,    71,    72,
      73,    75,    79,   101,   103,   104,   107,   108,   103,    82,
       0,    74,    87,    92,    93,    91,    90,    92,    94,    80,
      81,   103,   103,    70,    70,    70,    70,    70,    70,    70,
      70,    70,    70,    70,    12,    63,    70,   107,    70,    70,
      70,    70,    70,    70,    70,    70,    70,    70,    70,    70,
      70,    70,    70,    70,    79,   110,   111,   103,   107,    70,
      76,    77,    78,     3,     4,     7,     8,     9,    10,    11,
      80,    18,    21,    22,    35,    36,    41,    59,    66,    67,
      70,    80,    95,    80,    90,    80,    20,    56,    57,    97,
      98,    99,   100,    13,   107,   107,   107,    75,    75,   107,
      75,    75,    75,    75,    75,   103,   110,   107,   107,   107,
     107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     103,   107,   107,   107,    66,    81,    62,    82,    83,    83,
     107,   103,   103,   103,   103,   103,   103,   103,   107,    75,
     107,    75,   107,   107,   107,   107,    67,   109,   107,    75,
     107,   107,    70,    98,    66,   103,   107,    82,    82,    82,
      82,    82,    83,    82,    82,    82,    82,    82,    13,    62,
      82,    82,    82,    82,    82,    82,    82,    82,    82,    82,
      82,    82,    83,    82,    82,    82,   103,   107,    97,   103,
     107,   111,    83,   107,    81,    84,    83,    75,    58,   103,
     107,    14,    15,    79,   105,    14,    15,   102,   107,   107,
     107,   106,   107,   107,   107,   107,   107,   107,   107,   107,
     107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     107,   107,   107,    75,    75,    75,    66,    84,    75,    83,
      98,   103,   103,   107,   107,    83,    83,    83,    82,    83,
      82,    82,    82,    82,    82,    82,    83,    83,    83,    83,
      83,    83,    83,    83,    83,    83,    83,    83,    83,    83,
      83,   103,   107,    19,    84,    17,    13,    17,    13,   107,
     107,   107,   107,   107,   107,   107,   107,   103,   107,    83,
      83,    83,    83,    83,    83,   105,   102
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    85,    86,    86,    87,    88,    88,    88,    88,    89,
      89,    89,    90,    90,    91,    92,    93,    93,    94,    94,
      94,    95,    95,    96,    97,    97,    97,    98,    99,   100,
     101,   102,   102,   103,   103,   103,   103,   103,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   104,
     105,   105,   105,   106,   106,   107,   107,   107,   107,   107,
     107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     107,   107,   107,   107,   108,   109,   109,   110,   110,   111,
     111,   111,   111
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     2,     0,     1,     2,     2,     2,
       1,     3,     1,     2,     3,     3,     2,     3,     3,     5,
       5,     1,     3,     1,     1,     1,     1,     4,     1,     4,
       5,     3,     5,     3,     1,     4,     6,     3,     3,     2,
       3,     3,     3,     3,     3,     3,     3,     6,     6,     6,
       6,     6,     6,     6,     6,     1,     1,     1,     4,     5,
       3,     5,     1,     1,     3,     1,     3,     1,     4,     2,
       2,     2,     4,     4,     6,     2,     3,     3,     6,     6,
       6,     6,     6,     6,     3,     3,     3,     3,     3,     6,
       6,     6,     6,     8,     4,     8,     8,     8,     8,     8,
       1,     1,     4,     1,     3,     5,     7,     1,     3,     3,
       5,     3,     5
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
#line 206 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  GlobalParserInterface->letMgr->_parser_symbol_table.clear();
}
#line 1828 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 3:
#line 210 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  GlobalParserInterface->letMgr->_parser_symbol_table.clear(); 
}
#line 1836 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 4:
#line 216 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  GlobalParserInterface->getUserFlags().print_counterexample_flag = true;
  (GlobalSTP->Ctr_Example)->PrintCounterExample(true);
}
#line 1845 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 6:
#line 238 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    { 
  ((ASTVec*)AssertsQuery)->push_back(GlobalParserInterface->CreateNode(TRUE));
  ((ASTVec*)AssertsQuery)->push_back(*(yyvsp[0].node));                       
  delete (yyvsp[0].node);
}
#line 1855 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 7:
#line 244 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    { 
  ((ASTVec*)AssertsQuery)->push_back(GlobalParserInterface->CreateNode(TRUE));
  ((ASTVec*)AssertsQuery)->push_back(*(yyvsp[0].node));
  delete (yyvsp[0].node);
}
#line 1865 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 8:
#line 250 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  ASTVec aaa = GlobalParserInterface->GetAsserts();
  if(aaa.size() == 0)
    {
      yyerror("Fatal Error: parsing:  GetAsserts() call: no assertions: ");
    }

  ASTNode asserts = 
    aaa.size() == 1 ? 
    aaa[0] :
    GlobalParserInterface->CreateNode(AND, aaa);
  ((ASTVec*)AssertsQuery)->push_back(asserts);
  ((ASTVec*)AssertsQuery)->push_back(*(yyvsp[0].node));
  delete (yyvsp[0].node);
}
#line 1885 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 9:
#line 268 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  delete (yyvsp[0].vec);
}
#line 1893 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 10:
#line 272 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  delete (yyvsp[0].vec);
}
#line 1901 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 11:
#line 276 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  delete (yyvsp[0].vec);
}
#line 1909 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 12:
#line 296 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyval.vec) = new ASTVec;
  (yyval.vec)->push_back(*(yyvsp[0].node));
  GlobalParserInterface->AddAssert(*(yyvsp[0].node));
  delete (yyvsp[0].node);
}
#line 1920 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 13:
#line 303 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyvsp[-1].vec)->push_back(*(yyvsp[0].node));
  GlobalParserInterface->AddAssert(*(yyvsp[0].node));
  (yyval.vec) = (yyvsp[-1].vec);
  delete (yyvsp[0].node);
}
#line 1931 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 14:
#line 312 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    { 
  (yyval.node) = (yyvsp[-1].node);
 }
#line 1939 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 15:
#line 317 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    { GlobalParserInterface->SetQuery(*(yyvsp[-1].node)); (yyval.node) = (yyvsp[-1].node);}
#line 1945 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 16:
#line 323 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
}
#line 1952 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 17:
#line 326 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
}
#line 1959 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 18:
#line 331 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  for(vector<char*>::iterator i=(yyvsp[-2].stringVec)->begin(),iend=(yyvsp[-2].stringVec)->end();i!=iend;i++) {
    ASTNode s = stp::GlobalParserInterface->LookupOrCreateSymbol(*i);
    s.SetIndexWidth((yyvsp[0].indexvaluewidth).indexwidth);
    s.SetValueWidth((yyvsp[0].indexvaluewidth).valuewidth);
    GlobalParserInterface->letMgr->_parser_symbol_table.insert(s);
    GlobalParserBM->ListOfDeclaredVars.push_back(s);
  }
  delete (yyvsp[-2].stringVec);
}
#line 1974 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 19:
#line 342 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  //do type checking. if doesn't pass then abort
  BVTypeCheck(*(yyvsp[0].node));
  if((yyvsp[-2].indexvaluewidth).indexwidth != (yyvsp[0].node)->GetIndexWidth())
    yyerror("Fatal Error: parsing: LET Expr: Type check fail: ");
  if((yyvsp[-2].indexvaluewidth).valuewidth != (yyvsp[0].node)->GetValueWidth())
    yyerror("Fatal Error: parsing: LET Expr: Type check fail: ");
                         
  for(vector<char*>::iterator i=(yyvsp[-4].stringVec)->begin(),iend=(yyvsp[-4].stringVec)->end();i!=iend;i++) {                         
    GlobalParserInterface->letMgr->LetExprMgr(*i,*(yyvsp[0].node));
  }
    delete (yyvsp[0].node);
    delete (yyvsp[-4].stringVec);
}
#line 1993 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 20:
#line 357 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  //do type checking. if doesn't pass then abort
  BVTypeCheck(*(yyvsp[0].node));
  if((yyvsp[-2].indexvaluewidth).indexwidth != (yyvsp[0].node)->GetIndexWidth())
    yyerror("Fatal Error: parsing: LET Expr: Type check fail: ");
  if((yyvsp[-2].indexvaluewidth).valuewidth != (yyvsp[0].node)->GetValueWidth())
    yyerror("Fatal Error: parsing: LET Expr: Type check fail: ");
                         
  for(vector<char*>::iterator i=(yyvsp[-4].stringVec)->begin(),iend=(yyvsp[-4].stringVec)->end();i!=iend;i++) {                         
    GlobalParserInterface->letMgr->LetExprMgr(*i,*(yyvsp[0].node));
  }
  delete (yyvsp[0].node);
  delete (yyvsp[-4].stringVec);
}
#line 2012 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 21:
#line 374 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyval.stringVec) = new vector<char*>();                      
  (yyval.stringVec)->push_back((yyvsp[0].str));
 // delete $1;
}
#line 2022 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 22:
#line 380 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyvsp[0].stringVec)->push_back((yyvsp[-2].str));
  (yyval.stringVec) = (yyvsp[0].stringVec);
 // delete $1;
}
#line 2032 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 23:
#line 388 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyval.stringVec) = new vector<char*>((yyvsp[0].stringVec)->rbegin(),(yyvsp[0].stringVec)->rend());
  delete (yyvsp[0].stringVec);
}
#line 2041 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 24:
#line 403 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    { (yyval.indexvaluewidth) = (yyvsp[0].indexvaluewidth); }
#line 2047 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 25:
#line 404 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    { (yyval.indexvaluewidth) = (yyvsp[0].indexvaluewidth); }
#line 2053 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 26:
#line 405 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    { (yyval.indexvaluewidth) = (yyvsp[0].indexvaluewidth); }
#line 2059 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 27:
#line 409 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  /*((indexwidth is 0) && (valuewidth>0)) iff type is BV*/
  (yyval.indexvaluewidth).indexwidth = 0;
  unsigned int length = (yyvsp[-1].uintval);
  if(length > 0) {
    (yyval.indexvaluewidth).valuewidth = length;
  }
  else
    FatalError("Fatal Error: parsing: BITVECTORS must be of positive length: \n");
}
#line 2074 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 28:
#line 421 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyval.indexvaluewidth).indexwidth = 0;
  (yyval.indexvaluewidth).valuewidth = 0;
}
#line 2083 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 29:
#line 427 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyval.indexvaluewidth).indexwidth = (yyvsp[-2].indexvaluewidth).valuewidth;
  (yyval.indexvaluewidth).valuewidth = (yyvsp[0].indexvaluewidth).valuewidth;
}
#line 2092 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 30:
#line 435 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-1].node)->GetValueWidth();
  if (width != (yyvsp[0].node)->GetValueWidth())
    yyerror("Width mismatch in IF-THEN-ELSE");                   
  if((yyvsp[-1].node)->GetIndexWidth() != (yyvsp[0].node)->GetIndexWidth())
    yyerror("Width mismatch in IF-THEN-ELSE");

  BVTypeCheck(*(yyvsp[-3].node));
  BVTypeCheck(*(yyvsp[-1].node));
  BVTypeCheck(*(yyvsp[0].node));
  (yyval.node) = new ASTNode(GlobalParserInterface->nf->CreateArrayTerm(ITE,(yyvsp[0].node)->GetIndexWidth(), width, *(yyvsp[-3].node), *(yyvsp[-1].node), *(yyvsp[0].node)));
  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
  delete (yyvsp[0].node);
}
#line 2112 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 31:
#line 452 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[-1].node); }
#line 2118 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 32:
#line 454 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-3].node)->GetValueWidth();
  if (width != (yyvsp[-1].node)->GetValueWidth() || width != (yyvsp[0].node)->GetValueWidth())
    yyerror("Width mismatch in IF-THEN-ELSE");
  if ((yyvsp[-3].node)->GetIndexWidth() != (yyvsp[-1].node)->GetValueWidth() || (yyvsp[-3].node)->GetIndexWidth() != (yyvsp[0].node)->GetValueWidth())
    yyerror("Width mismatch in IF-THEN-ELSE");

  BVTypeCheck(*(yyvsp[-3].node));
  BVTypeCheck(*(yyvsp[-1].node));
  BVTypeCheck(*(yyvsp[0].node));                     
  (yyval.node) = new ASTNode(GlobalParserInterface->nf->CreateArrayTerm(ITE, (yyvsp[0].node)->GetIndexWidth(), width, *(yyvsp[-3].node), *(yyvsp[-1].node), *(yyvsp[0].node)));
  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
  delete (yyvsp[0].node);
}
#line 2138 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 33:
#line 473 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyval.node) = (yyvsp[-1].node); 
}
#line 2146 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 34:
#line 477 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {  
  (yyval.node) = new ASTNode(GlobalParserInterface->letMgr->ResolveID(*(yyvsp[0].node))); delete (yyvsp[0].node);
}
#line 2154 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 35:
#line 481 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode(GlobalParserInterface->nf->CreateNode(PARAMBOOL,*(yyvsp[-3].node),*(yyvsp[-1].node)));
  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2164 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 36:
#line 487 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-3].node)->GetValueWidth();
  if(width <= (unsigned)(yyvsp[-1].uintval))
    yyerror("Fatal Error: BOOLEXTRACT: trying to boolextract a bit which beyond range");
                         
  ASTNode bit = GlobalParserInterface->CreateBVConst(32, (yyvsp[-1].uintval));
  ASTNode * out = new ASTNode(GlobalParserInterface->nf->CreateNode(BOOLEXTRACT,*(yyvsp[-3].node),bit));

  (yyval.node) = out;
  delete (yyvsp[-3].node);
}
#line 2180 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 37:
#line 499 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateNode(EQ, *(yyvsp[-2].node), *(yyvsp[0].node)));
  (yyval.node) = n;
  delete (yyvsp[-2].node);
  delete (yyvsp[0].node);
}
#line 2191 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 38:
#line 506 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateNode(NOT, GlobalParserInterface->nf->CreateNode(EQ, *(yyvsp[-2].node), *(yyvsp[0].node))));
  (yyval.node) = n;
  delete (yyvsp[-2].node);
  delete (yyvsp[0].node);
}
#line 2202 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 39:
#line 513 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode(GlobalParserInterface->nf->CreateNode(NOT, *(yyvsp[0].node)));
  delete (yyvsp[0].node);
}
#line 2211 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 40:
#line 518 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode(GlobalParserInterface->nf->CreateNode(OR, *(yyvsp[-2].node), *(yyvsp[0].node)));
  delete (yyvsp[-2].node);
  delete (yyvsp[0].node);
}
#line 2221 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 41:
#line 524 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode(GlobalParserInterface->nf->CreateNode(NOR, *(yyvsp[-2].node), *(yyvsp[0].node)));
  delete (yyvsp[-2].node);
  delete (yyvsp[0].node);
}
#line 2231 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 42:
#line 530 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode(GlobalParserInterface->nf->CreateNode(AND, *(yyvsp[-2].node), *(yyvsp[0].node)));
  delete (yyvsp[-2].node);
  delete (yyvsp[0].node);
}
#line 2241 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 43:
#line 536 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode(GlobalParserInterface->nf->CreateNode(NAND, *(yyvsp[-2].node), *(yyvsp[0].node)));
  delete (yyvsp[-2].node);
  delete (yyvsp[0].node);
}
#line 2251 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 44:
#line 542 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode(GlobalParserInterface->nf->CreateNode(IMPLIES, *(yyvsp[-2].node), *(yyvsp[0].node)));
  delete (yyvsp[-2].node);
  delete (yyvsp[0].node);
}
#line 2261 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 45:
#line 548 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode(GlobalParserInterface->nf->CreateNode(IFF, *(yyvsp[-2].node), *(yyvsp[0].node)));
  delete (yyvsp[-2].node);
  delete (yyvsp[0].node);
}
#line 2271 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 46:
#line 554 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode(GlobalParserInterface->nf->CreateNode(XOR, *(yyvsp[-2].node), *(yyvsp[0].node)));
  delete (yyvsp[-2].node);
  delete (yyvsp[0].node);
}
#line 2281 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 47:
#line 560 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateNode(BVLT, *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;
  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2292 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 48:
#line 567 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateNode(BVGT, *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;
  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2303 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 49:
#line 574 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateNode(BVLE, *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;
  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2314 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 50:
#line 581 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateNode(BVGE, *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;
  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2325 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 51:
#line 588 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateNode(BVSLT, *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;
  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2336 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 52:
#line 595 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateNode(BVSGT, *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;
  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2347 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 53:
#line 602 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateNode(BVSLE, *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;
  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2358 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 54:
#line 609 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateNode(BVSGE, *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;
  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2369 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 56:
#line 617 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode(GlobalParserInterface->CreateNode(TRUE)); 
  assert((yyval.node)->GetIndexWidth() == 0);
  assert((yyval.node)->GetValueWidth() == 0);
}
#line 2379 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 57:
#line 623 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    { 
  (yyval.node) = new ASTNode(GlobalParserInterface->CreateNode(FALSE)); 
  assert((yyval.node)->GetIndexWidth() == 0);
  assert((yyval.node)->GetValueWidth() == 0);
}
#line 2389 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 58:
#line 630 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyval.node) = (yyvsp[0].node);
  //Cleanup the LetIDToExprMap
  GlobalParserInterface->letMgr->CleanupLetIDMap();
}
#line 2399 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 59:
#line 639 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode(GlobalParserInterface->nf->CreateNode(ITE, *(yyvsp[-3].node), *(yyvsp[-1].node), *(yyvsp[0].node)));
  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
  delete (yyvsp[0].node);
}
#line 2410 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 60:
#line 647 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[-1].node); }
#line 2416 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 61:
#line 649 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyval.node) = new ASTNode(GlobalParserInterface->nf->CreateNode(ITE, *(yyvsp[-3].node), *(yyvsp[-1].node), *(yyvsp[0].node)));
  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
  delete (yyvsp[0].node);
}
#line 2427 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 62:
#line 655 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
   cerr << "Unresolved symbol:" << (yyvsp[0].str) << endl;
   yyerror("bad symbol"); 
}
#line 2436 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 63:
#line 663 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyval.vec) = new ASTVec;
  BVTypeCheck(*(yyvsp[0].node));
  (yyval.vec)->push_back(*(yyvsp[0].node));
  delete (yyvsp[0].node);
}
#line 2447 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 64:
#line 670 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyvsp[-2].vec)->push_back(*(yyvsp[0].node));
  BVTypeCheck(*(yyvsp[0].node));
  (yyval.vec) = (yyvsp[-2].vec); 
  delete (yyvsp[0].node);
}
#line 2458 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 65:
#line 679 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    { (yyval.node) = new ASTNode(GlobalParserInterface->letMgr->ResolveID(*(yyvsp[0].node))); delete (yyvsp[0].node);}
#line 2464 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 66:
#line 680 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[-1].node); }
#line 2470 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 67:
#line 681 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2476 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 68:
#line 683 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  BVTypeCheck(*(yyvsp[-1].node));
  ASTNode one = GlobalParserInterface->CreateBVConst(1,1);
  ASTNode zero = GlobalParserInterface->CreateBVConst(1,0);

  //return ITE(*$3, length(1), 0bin1, 0bin0)
  (yyval.node) = new ASTNode(GlobalParserInterface->nf->CreateTerm(ITE,1,*(yyvsp[-1].node),one,zero));
  delete (yyvsp[-1].node);
}
#line 2490 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 69:
#line 693 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    { 
  std::string vals((yyvsp[0].str));
  (yyval.node) = new ASTNode(GlobalParserInterface->CreateBVConst(vals, 2, (yyvsp[-1].uintval)));
  free((yyvsp[0].str));
}
#line 2500 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 70:
#line 699 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    { 
  std::string vals((yyvsp[0].str));
  (yyval.node) = new ASTNode(GlobalParserInterface->CreateBVConst(vals, 10, (yyvsp[-1].uintval)));
  free((yyvsp[0].str));
}
#line 2510 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 71:
#line 705 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    { 
  std::string vals((yyvsp[0].str));
  (yyval.node) = new ASTNode(GlobalParserInterface->CreateBVConst(vals, 16, (yyvsp[-1].uintval)));
  free((yyvsp[0].str));
}
#line 2520 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 72:
#line 711 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {                        
  // valuewidth is same as array, indexwidth is 0.
  unsigned int width = (yyvsp[-3].node)->GetValueWidth();
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(READ, width, *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;

  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2534 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 73:
#line 721 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  // valuewidth is same as array, indexwidth is 0.
  unsigned int width = (yyvsp[-3].node)->GetValueWidth();
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(READ, width, *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;

  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2548 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 74:
#line 731 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  int width = (yyvsp[-3].uintval) - (yyvsp[-1].uintval) + 1;
  if (width < 0)
    yyerror("Negative width in extract");
                         
  if((unsigned)(yyvsp[-3].uintval) >= (yyvsp[-5].node)->GetValueWidth())
    yyerror("Parsing: Wrong width in BVEXTRACT\n");                      

  ASTNode hi  =  GlobalParserInterface->CreateBVConst(32, (yyvsp[-3].uintval));
  ASTNode low =  GlobalParserInterface->CreateBVConst(32, (yyvsp[-1].uintval));
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVEXTRACT, width, *(yyvsp[-5].node),hi,low));
  (yyval.node) = n;
  delete (yyvsp[-5].node);
}
#line 2567 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 75:
#line 746 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[0].node)->GetValueWidth();
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVNOT, width, *(yyvsp[0].node)));
  (yyval.node) = n;
  delete (yyvsp[0].node);
}
#line 2578 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 76:
#line 753 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-2].node)->GetValueWidth();
  if (width != (yyvsp[0].node)->GetValueWidth()) {
    yyerror("Width mismatch in AND");
  }
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVAND, width, *(yyvsp[-2].node), *(yyvsp[0].node)));
  (yyval.node) = n;
  delete (yyvsp[-2].node);
  delete (yyvsp[0].node);
}
#line 2593 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 77:
#line 764 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-2].node)->GetValueWidth();
  if (width != (yyvsp[0].node)->GetValueWidth()) {
    yyerror("Width mismatch in OR");
  }
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVOR, width, *(yyvsp[-2].node), *(yyvsp[0].node))); 
  (yyval.node) = n;
  delete (yyvsp[-2].node);
  delete (yyvsp[0].node);
}
#line 2608 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 78:
#line 775 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-3].node)->GetValueWidth();
  if (width != (yyvsp[-1].node)->GetValueWidth()) {
    yyerror("Width mismatch in XOR");
  }
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVXOR, width, *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;
  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2623 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 79:
#line 786 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-3].node)->GetValueWidth();
  if (width != (yyvsp[-1].node)->GetValueWidth()) {
    yyerror("Width mismatch in NAND");
  }
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVNAND, width, *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;

  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2639 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 80:
#line 798 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-3].node)->GetValueWidth();
  if (width != (yyvsp[-1].node)->GetValueWidth()) {
    yyerror("Width mismatch in NOR");
  }
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVNOR, width, *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;

  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2655 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 81:
#line 810 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-3].node)->GetValueWidth();
  if (width != (yyvsp[-1].node)->GetValueWidth()) {
    yyerror("Width mismatch in NOR");
  }
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVXNOR, width, *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;

  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2671 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 82:
#line 822 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  //width of the expr which is being sign
  //extended. $5 is the resulting length of the
  //sign-extended expr
  BVTypeCheck(*(yyvsp[-3].node));
  ASTNode width = GlobalParserInterface->CreateBVConst(32,(yyvsp[-1].uintval));
  ASTNode *n =
    new ASTNode(GlobalParserInterface->nf->CreateTerm(BVSX, (yyvsp[-1].uintval),*(yyvsp[-3].node),width));
  (yyval.node) = n;
  delete (yyvsp[-3].node);
}
#line 2687 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 83:
#line 834 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  //width of the expr which is being zero
  //extended. $5 is the resulting length of the
  //zero-extended expr
  BVTypeCheck(*(yyvsp[-3].node));
  ASTNode width = GlobalParserInterface->CreateBVConst(32,(yyvsp[-1].uintval));
  ASTNode *n =
    new ASTNode(GlobalParserInterface->nf->CreateTerm(BVZX, (yyvsp[-1].uintval),*(yyvsp[-3].node),width));
  (yyval.node) = n;
  delete (yyvsp[-3].node);
}
#line 2703 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 84:
#line 846 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-2].node)->GetValueWidth() + (yyvsp[0].node)->GetValueWidth();
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVCONCAT, width, *(yyvsp[-2].node), *(yyvsp[0].node)));
  (yyval.node) = n;
                         
  delete (yyvsp[-2].node);
  delete (yyvsp[0].node);
}
#line 2716 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 85:
#line 855 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  if (0 == (yyvsp[0].uintval))
  	{
  	(yyval.node) = (yyvsp[-2].node);
  	}
  else
  {
  ASTNode zero_bits = GlobalParserInterface->CreateZeroConst((yyvsp[0].uintval));
  ASTNode * n = 
    new ASTNode(GlobalParserInterface->nf->CreateTerm(BVCONCAT,
                                     (yyvsp[-2].node)->GetValueWidth() + (yyvsp[0].uintval), *(yyvsp[-2].node), zero_bits));
  (yyval.node) = n;
  delete (yyvsp[-2].node);
  }
}
#line 2736 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 86:
#line 871 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  // VARIABLE LEFT SHIFT
  //
  // $1 (THEEXPR) is being shifted
  //
  // $3 is the variable shift amount
  unsigned int width = (yyvsp[-2].node)->GetValueWidth();
  ASTNode * ret = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVLEFTSHIFT, width, *(yyvsp[-2].node), *(yyvsp[0].node)));
  BVTypeCheck(*ret);
  //cout << *ret;

  (yyval.node) = ret;
  delete (yyvsp[-2].node);
  delete (yyvsp[0].node);
}
#line 2756 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 87:
#line 887 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  ASTNode len = GlobalParserInterface->CreateZeroConst((yyvsp[0].uintval));
  unsigned int w = (yyvsp[-2].node)->GetValueWidth();

  //the amount by which you are rightshifting
  //is less-than/equal-to the length of input
  //bitvector
  if((unsigned)(yyvsp[0].uintval) < w) {
    ASTNode hi = GlobalParserInterface->CreateBVConst(32,w-1);
    ASTNode low = GlobalParserInterface->CreateBVConst(32,(yyvsp[0].uintval));
    ASTNode extract = GlobalParserInterface->nf->CreateTerm(BVEXTRACT,w-(yyvsp[0].uintval),*(yyvsp[-2].node),hi,low);
    ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVCONCAT, w,len, extract));
    (yyval.node) = n;
  }
  else
    (yyval.node) = new ASTNode(GlobalParserInterface->CreateZeroConst(w));

  delete (yyvsp[-2].node);
}
#line 2780 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 88:
#line 907 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  // VARIABLE RIGHT SHIFT
  //
  // $1 (THEEXPR) is being shifted
  //
  // $3 is the variable shift amount
  unsigned int width = (yyvsp[-2].node)->GetValueWidth();
  ASTNode * ret = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVRIGHTSHIFT, width, *(yyvsp[-2].node), *(yyvsp[0].node)));
  BVTypeCheck(*ret);
  //cout << *ret;

  (yyval.node) = ret;
  delete (yyvsp[-2].node);
  delete (yyvsp[0].node);
}
#line 2800 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 89:
#line 923 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-3].node)->GetValueWidth();
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVLEFTSHIFT, width, *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;

  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2813 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 90:
#line 932 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-3].node)->GetValueWidth();
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVRIGHTSHIFT, width, *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;

  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2826 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 91:
#line 941 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  unsigned int width = (yyvsp[-3].node)->GetValueWidth();
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVSRSHIFT, width, *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;

  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2839 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 92:
#line 950 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVPLUS, (yyvsp[-3].uintval), *(yyvsp[-1].vec)));
  (yyval.node) = n;

  delete (yyvsp[-1].vec);
}
#line 2850 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 93:
#line 957 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVSUB, (yyvsp[-5].uintval), *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;

  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2862 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 94:
#line 965 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  unsigned width = (yyvsp[-1].node)->GetValueWidth();
  ASTNode * n =  new ASTNode(GlobalParserInterface->nf->CreateTerm(BVUMINUS,width,*(yyvsp[-1].node)));
  (yyval.node) = n;
  delete (yyvsp[-1].node);
}
#line 2873 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 95:
#line 972 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVMULT, (yyvsp[-5].uintval), *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;

  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2885 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 96:
#line 980 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVDIV, (yyvsp[-5].uintval), *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;

  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2897 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 97:
#line 988 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(BVMOD, (yyvsp[-5].uintval), *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;

  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2909 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 98:
#line 996 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(SBVDIV, (yyvsp[-5].uintval), *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;

  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2921 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 99:
#line 1004 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  ASTNode * n = new ASTNode(GlobalParserInterface->nf->CreateTerm(SBVREM, (yyvsp[-5].uintval), *(yyvsp[-3].node), *(yyvsp[-1].node)));
  (yyval.node) = n;
  delete (yyvsp[-3].node);
  delete (yyvsp[-1].node);
}
#line 2932 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 100:
#line 1010 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    { (yyval.node) = (yyvsp[0].node); }
#line 2938 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 102:
#line 1013 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyval.node) = (yyvsp[0].node);
}
#line 2946 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 103:
#line 1016 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
   cerr << "Unresolved symbol:" << (yyvsp[0].str) << endl;
   yyerror("bad symbol"); 
}
#line 2955 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 104:
#line 1024 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  ASTNode * result;
  unsigned int width = (yyvsp[-2].node)->GetValueWidth();

  ASTNodeMap::iterator it = (yyvsp[0].Index_To_UpdateValue)->begin();
  ASTNodeMap::iterator itend = (yyvsp[0].Index_To_UpdateValue)->end();
  result = new ASTNode(GlobalParserInterface->nf->CreateArrayTerm(WRITE,
                                            (yyvsp[-2].node)->GetIndexWidth(),
                                            width,
                                            *(yyvsp[-2].node),
                                            (*it).first,
                                            (*it).second));
  BVTypeCheck(*result);
  for(it++;it!=itend;it++) {
    result = new ASTNode(GlobalParserInterface->nf->CreateArrayTerm(WRITE,
                                              (yyvsp[-2].node)->GetIndexWidth(),
                                              width,
                                              *result,
                                              (*it).first,
                                              (*it).second));
    BVTypeCheck(*result);
  }
  BVTypeCheck(*result);
  (yyval.node) = result;
  delete (yyvsp[0].Index_To_UpdateValue);
  delete (yyvsp[-2].node);
}
#line 2987 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 105:
#line 1054 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  (yyval.Index_To_UpdateValue) = new ASTNodeMap();
  (*(yyval.Index_To_UpdateValue))[*(yyvsp[-3].node)] = *(yyvsp[0].node);         
  delete (yyvsp[-3].node);
  delete (yyvsp[0].node);        
}
#line 2998 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 106:
#line 1061 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {                   
  (*(yyvsp[-6].Index_To_UpdateValue))[*(yyvsp[-3].node)] = *(yyvsp[0].node);
  delete (yyvsp[-3].node);
  delete (yyvsp[0].node);
}
#line 3008 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 109:
#line 1074 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  //Expr must typecheck
  BVTypeCheck(*(yyvsp[0].node));

  //set the valuewidth of the identifier
  
  //populate the hashtable from LET-var -->
  //LET-exprs and then process them:
  //
  //1. ensure that LET variables do not clash
  //1. with declared variables.
  //
  //2. Ensure that LET variables are not
  //2. defined more than once
  GlobalParserInterface->letMgr->LetExprMgr((yyvsp[-2].str),*(yyvsp[0].node));
  free((yyvsp[-2].str));
  delete (yyvsp[0].node);
}
#line 3031 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 110:
#line 1093 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  //do type checking. if doesn't pass then abort
  BVTypeCheck(*(yyvsp[0].node));
                          
  if((yyvsp[-2].indexvaluewidth).indexwidth != (yyvsp[0].node)->GetIndexWidth())
    yyerror("Fatal Error: parsing: LET Expr: Type check fail: ");
  if((yyvsp[-2].indexvaluewidth).valuewidth != (yyvsp[0].node)->GetValueWidth())
    yyerror("Fatal Error: parsing: LET Expr: Type check fail: ");

  GlobalParserInterface->letMgr->LetExprMgr((yyvsp[-4].str),*(yyvsp[0].node));
  free( (yyvsp[-4].str));
  delete (yyvsp[0].node);
}
#line 3049 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 111:
#line 1107 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  //Expr must typecheck
  BVTypeCheck(*(yyvsp[0].node));

  //Do LET-expr management
  GlobalParserInterface->letMgr->LetExprMgr((yyvsp[-2].str),*(yyvsp[0].node));
  free( (yyvsp[-2].str));
  delete (yyvsp[0].node);
}
#line 3063 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;

  case 112:
#line 1117 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1646  */
    {
  //do type checking. if doesn't pass then abort
  BVTypeCheck(*(yyvsp[0].node));

  if((yyvsp[-2].indexvaluewidth).indexwidth != (yyvsp[0].node)->GetIndexWidth())
    yyerror("Fatal Error: parsing: LET Expr: Type check fail: ");
  if((yyvsp[-2].indexvaluewidth).valuewidth != (yyvsp[0].node)->GetValueWidth())
    yyerror("Fatal Error: parsing: LET Expr: Type check fail: ");

  //Do LET-expr management
  GlobalParserInterface->letMgr->LetExprMgr((yyvsp[-4].str),*(yyvsp[0].node));
  free( (yyvsp[-4].str));
  delete (yyvsp[0].node);
}
#line 3082 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
    break;


#line 3086 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.cpp" /* yacc.c:1646  */
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
#line 1133 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1906  */

