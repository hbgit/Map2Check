#pragma once
#include <string>

namespace Map2Check::Log {
  void initLog();
  void Error(std::string msg);
  void Warning(std::string msg);
  void Debug(std::string msg);
  void Info(std::string msg);  
  void Fatal(std::string msg);  
}
						   
