#ifndef CALLER_H
#define CALLER_H

#include <iostream>
#include <string>

/*#include <llvm/IR/Module.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>*/

class Caller {

	protected:
      std::string pathprogram;

   public:
      Caller( std::string bcprogram_path );

      void printdata();

      int parseIrFile();

			void genByteCodeFile();

      int callPass();
};

#endif
