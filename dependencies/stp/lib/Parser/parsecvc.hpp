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
#line 58 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/cvc.y" /* yacc.c:1909  */


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

#line 152 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsecvc.hpp" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE cvclval;

int cvcparse (void* AssertsQuery);

#endif /* !YY_CVC_HOME_MAP2CHECK_DEVEL_TOOL_MAP_SRC_ON_DOCKER_DEPENDENCIES_STP_LIB_PARSER_PARSECVC_HPP_INCLUDED  */
