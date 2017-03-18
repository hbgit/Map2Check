#include <boost/log/trivial.hpp>
#include <string>

namespace Map2Check::Log {
  void BoostError(std::string msg) {
    BOOST_LOG_TRIVIAL(error) << "\033[0;31m" << msg << "\033[0m";
  }

  void BoostWarning(std::string msg) {
    BOOST_LOG_TRIVIAL(warning) << "\033[1;33m" << msg << "\033[0m";
  }

  void BoostDebug(std::string msg) {
    BOOST_LOG_TRIVIAL(debug) << "\033[0;33m" << msg << "\033[0m";
  }

  void BoostInfo(std::string msg) {
    BOOST_LOG_TRIVIAL(info) << "\033[0;32m" << msg << "\033[0m";
  }
  
  void BoostFatal(std::string msg) {
    BOOST_LOG_TRIVIAL(fatal) << "\033[0;31m" << msg << "\033[0m";
  }

  void TestColors() {
    BoostWarning("This is a warning message!");
    BoostError("This is an error message!");
    BoostDebug("This is a debug message!");
    BoostInfo("This is an info message!");
    BoostFatal("This is a fatal message!");
  }

  
}
						   
