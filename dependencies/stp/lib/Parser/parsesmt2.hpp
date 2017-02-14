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
#line 76 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/smt2.y" /* yacc.c:1909  */
  
  unsigned uintval;                  /* for numerals in types. */
  //ASTNode,ASTVec
  ASTNode *node;
  ASTVec *vec;
  std::string *str;

#line 166 "/home/map2check/devel_tool/map_src_on_docker/dependencies/stp/lib/Parser/parsesmt2.hpp" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE smt2lval;

int smt2parse (void);

#endif /* !YY_SMT2_HOME_MAP2CHECK_DEVEL_TOOL_MAP_SRC_ON_DOCKER_DEPENDENCIES_STP_LIB_PARSER_PARSESMT2_HPP_INCLUDED  */
