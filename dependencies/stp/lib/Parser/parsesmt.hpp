/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

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
#line 70 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt.y" /* yacc.c:1909  */
  
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

#line 180 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt.hpp" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE smtlval;

int smtparse (void* AssertsQuery);

#endif /* !YY_SMT_HOME_MAP2CHECK_DEVEL_TOOL_MAP_SRC_ON_DOCKER_DEPENDENCIES_STP_LIB_PARSER_PARSESMT_HPP_INCLUDED  */
