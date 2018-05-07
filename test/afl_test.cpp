#define BOOST_TEST_MODULE AFL_TESTS
#include <boost/test/included/unit_test.hpp>
#include <modules/frontend/utils/afl.hpp>

// Test for default constructor
BOOST_AUTO_TEST_CASE(constructors) {
  std::string path = "/my-custom-path/";
  std::string expectedCompilerCommand =
    path + "afl-clang";

  std::string expectedExecuterCommand =
    path + "afl-fuzz";

  Map2Check::AFL afl(path);
  BOOST_CHECK_EQUAL(afl.getCompilerCommand(), expectedCompilerCommand);
  BOOST_CHECK_EQUAL(afl.getExecutionCommand(), expectedExecuterCommand);
}

BOOST_AUTO_TEST_CASE(_32Bits_Compiler) {
  std::string path = "/my-custom-path/";
  std::string expectedCompilerCommand =
    path + "afl-clang -m32";

  Map2Check::AFL_COMPILE compiler;
  compiler.setPath(path);
  compiler.setCompileTo32Bits(true);

  BOOST_CHECK_EQUAL(expectedCompilerCommand, compiler.getCommand());
}

BOOST_AUTO_TEST_CASE(_Timeout_Executor) {
  std::string path = "/my-custom-path/";
  std::string expectedExecutorCommand =
    "timeout 30 " + path + "afl-fuzz";

  Map2Check::AFL_EXEC executor;
  executor.setPath(path);
  executor.setTimeout(30);

  BOOST_CHECK_EQUAL(expectedExecutorCommand, executor.getCommand());
}

BOOST_AUTO_TEST_CASE(_Chrash_Executor) {
  std::string path = "/my-custom-path/";
  std::string expectedExecutorCommand =
    "AFL_BENCH_UNTIL_CRASH=1 " + path + "afl-fuzz";

  Map2Check::AFL_EXEC executor;
  executor.setPath(path);
  executor.setBenchUntilCrash(true);

  BOOST_CHECK_EQUAL(expectedExecutorCommand, executor.getCommand());
}

BOOST_AUTO_TEST_CASE(_Skip_Cpu_Executor) {
  std::string path = "/my-custom-path/";
  std::string expectedExecutorCommand =
    "AFL_SKIP_CPUFREQ=1 " + path + "afl-fuzz";

  Map2Check::AFL_EXEC executor;
  executor.setPath(path);
  executor.setSkipCpuFreq(true);

  BOOST_CHECK_EQUAL(expectedExecutorCommand, executor.getCommand());
}

BOOST_AUTO_TEST_CASE(_Change_Mode_Executor) {
  std::string path = "/my-custom-path/";
  std::string expectedExecutorCommand =
    "AFL_BENCH_UNTIL_CRASH=1 AFL_SKIP_CPUFREQ=1 timeout 30 " +
    path + "afl-fuzz";

  Map2Check::AFL_EXEC executor;
  executor.setPath(path);
  executor.setBenchUntilCrash(true);
  executor.setTimeout(30);
  executor.setSkipCpuFreq(true);

  BOOST_CHECK_EQUAL(expectedExecutorCommand, executor.getCommand());
}
