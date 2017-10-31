#include "BinaryOperation.h"

#include <limits.h>
#include "PropertyGenerator.h"
#include <stdio.h>

int absInteger (int a) {
    if (a < 0) {
        return (-a);
    }
    return a;
}

void overflowError(unsigned line, const char* function_name) {
    write_property_overflow(line, function_name);
    write_property(OVERFLOW, line, function_name);
    map2check_error();
}

void divisionByZeroError() {
    // TODO: implement method
}


void map2check_binop_add(unsigned param1, int param2,
			 unsigned line, unsigned scope,
			 char* function_name) {

  Bool param1Pos = (param1 >= 0);
  Bool param2Pos = (param2 >= 0);

  // Both are positive
  if(param1Pos && param2Pos) {
    // If param2 is greater thant the difference between max int and param1,
    // then we have an overflow    
    if((UINT_MAX - param1) < param2) {		
        overflowError(param1, "DELTA");
    }
  }
  // Both are negative
  else if(!param1Pos && !param2Pos) {
    // Same principle of first IF
    if((INT_MIN - param1) > param2) {		
        overflowError(param2, function_name);
        //overflowError(param1, function_name);
    }
  }
  else {
      return;
  }
}

//TODO is need to know when is a unsigned!!!
void map2check_binop_sub(unsigned param1, unsigned param2,
                         unsigned line, unsigned scope,
                         char* function_name) {
    map2check_binop_add(param1, -param2, line, scope, function_name );
}

void map2check_binop_mul(int param1, int param2,
                          unsigned line, unsigned scope,
                          char* function_name) {
    // If one of params is 0, the result will be 0
    if((param1 == 0) || (param2 == 0)) {
        return;
    }


    // If a parameter is 1, the result will not be an overflow

    if((param1 == 1) || (param2 == 1)) {
        return;
    }

    Bool isParam1Positive = (param1 >= 0);
    Bool isParam2Positive = (param2 >= 0);

    if((isParam1Positive && isParam2Positive) || (!isParam1Positive && !isParam2Positive)) {        
        int limitPositiveResult = INT_MAX/param1;

        switch (isParam1Positive) {
            case TRUE:
                if(limitPositiveResult < param2) {
                    overflowError(line, function_name);
                }
                break;
            case FALSE:
                if(param2 < limitPositiveResult) {
                    overflowError(line, function_name);
                }
                break;
        }

    }

    else {
        int limitNegativeResult = INT_MIN/param1;
        switch (isParam1Positive) {
            case TRUE:
                if(limitNegativeResult > param2) {
                    overflowError(line, function_name);
                }
                break;
            case FALSE:
	      if(param2 > absInteger(limitNegativeResult)) {
                    overflowError(line, function_name);
                }
                break;
        }
    }




}


void map2check_binop_sdiv(int param1, int param2,
                          unsigned line, unsigned scope,
                          char* function_name) {
    if(param2 == 0) {
        divisionByZeroError();
    }
    return;
}
