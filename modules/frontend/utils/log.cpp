#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
// #include <boost/log/sinks/sink.hpp>
#include <string>
#include <iostream>
#include "tools.hpp"

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;


namespace Map2Check::Log {

  void Error(std::string msg) {
    BOOST_LOG_TRIVIAL(error) << "\033[0;31m" << msg << "\033[0m";
  }

  void Warning(std::string msg) {
    BOOST_LOG_TRIVIAL(warning) << "\033[1;33m" << msg << "\033[0m";
  }

  void initLog() {
      logging::core::get()->set_filter(
              logging::trivial::severity >= logging::trivial::info);
  }

  void Debug(std::string msg) {
      BOOST_LOG_TRIVIAL(debug) << "\033[0;33m" << msg << "\033[0m";

  }

  void Info(std::string msg) {
    // BOOST_LOG_TRIVIAL(info) << "\033[0;32m" << msg << "\033[0m";
    std::cout << "\033[0;32m" << msg << "\033[0m" << std::endl;
  }

  void Fatal(std::string msg) {
    BOOST_LOG_TRIVIAL(fatal) << "\033[0;31m" << msg << "\033[0m";
  }


}
