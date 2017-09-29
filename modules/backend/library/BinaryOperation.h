/**
 * This module will check/analyze/validate all signed integer operations
 */

#include "Map2CheckTypes.h"

#ifndef BinaryOperation_H
#define BinaryOperation_H

/**
 * @brief Checks ADD operations and adds cases where operation would fail.
 * @param param1 Pointer to first operand
 * @param param1Type Type of first operand (for pointer cast)
 * @param param2 Pointer to second operand
 * @param param2Type Type of second operand (for pointer cast)
 */
void map2check_binop_add(void* param1, PRIMITIVE_TYPE param1Type,
                         void* param2, PRIMITIVE_TYPE param2Type);

/**
 * @brief Checks SUB operations and adds cases where operation would fail.
 * @param param1 Pointer to first operand
 * @param param1Type Type of first operand (for pointer cast)
 * @param param2 Pointer to second operand
 * @param param2Type Type of second operand (for pointer cast)
 */
void map2check_binop_sub(void* param1, PRIMITIVE_TYPE param1Type,
                         void* param2, PRIMITIVE_TYPE param2Type);

/**
 * @brief Checks MUL operations and adds cases where operation would fail.
 * @param param1 Pointer to first operand
 * @param param1Type Type of first operand (for pointer cast)
 * @param param2 Pointer to second operand
 * @param param2Type Type of second operand (for pointer cast)
 */
void map2check_binop_mul(void* param1, PRIMITIVE_TYPE param1Type,
                         void* param2, PRIMITIVE_TYPE param2Type);

/**
 * @brief Checks DIV operations and adds cases where operation would fail.
 * @param param1 Pointer to first operand
 * @param param1Type Type of first operand (for pointer cast)
 * @param param2 Pointer to second operand
 * @param param2Type Type of second operand (for pointer cast)
 */
void map2check_binop_div(void* param1, PRIMITIVE_TYPE param1Type,
                         void* param2, PRIMITIVE_TYPE param2Type);




#endif

