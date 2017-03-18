#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

/* enum class WitnessExceptions { */
/*   INVALID_FILE */
/* }; */
  
enum class WitnessException {
  INVALID_FILE
};


class Witness {
  public: 
    static void GenerateWitness(std::vector<std::string> kleeFiles);
    static void GenerateWitness(std::string kleeResultFolder); 
    static std::vector<std::string> listFilesFromFolder(std::string kleeResultFolder); 
  };
