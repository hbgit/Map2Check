#include <gtest/gtest.h>
extern "C" {
#include <modules/backend/library/header/NonDetLog.h>
}

#include <cfloat>
#include <climits>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

/* NonDetLog.c takes its step counter from Map2CheckFunctions.c; the test
 * links only the log and its container. */
extern "C" {
static unsigned current_step = 0;
unsigned map2check_get_current_step() { return current_step; }
void map2check_next_current_step() { current_step++; }
}

namespace {
std::string readFile(const std::string &path) {
  std::ifstream in(path);
  std::stringstream text;
  text << in.rdbuf();
  return text.str();
}
}  // namespace

class NonDetLogTest : public ::testing::Test {
 protected:
  void SetUp() override {
    previous = std::filesystem::current_path();
    directory = std::filesystem::temp_directory_path() /
                ("nondetlog-" + std::to_string(::getpid()));
    std::filesystem::create_directories(directory);
    std::filesystem::current_path(directory);
    current_step = 0;
    nondet_log_init();
  }
  void TearDown() override {
    nondet_log_destroy();
    std::filesystem::current_path(previous);
    std::filesystem::remove_all(directory);
  }

  std::filesystem::path previous;
  std::filesystem::path directory;
};

/* The writer formats numbers by hand, not with fprintf (issue #75: under KLEE
 * uClibc's vfprintf cost ~7,000 instructions a call). Every expected line
 * below is what the fprintf version printed for the same row, so the file the
 * frontend parses is byte for byte the same. */
TEST_F(NonDetLogTest, WritesEveryTypeExactlyAsPrintfDid) {
  map2check_nondet_int(10, 3, -1, "__VERIFIER_nondet_int");
  map2check_nondet_int(11, 3, INT_MIN, "__VERIFIER_nondet_int");
  map2check_nondet_unsigned(1, 2, UINT_MAX, "u");
  map2check_nondet_longlong(5, 6, LLONG_MIN, "ll");
  map2check_nondet_ulonglong(7, 8, ULLONG_MAX, "ull");
  map2check_nondet_double(9, 9, 1.5, "d");
  map2check_nondet_double(9, 9, -0.0, "d");
  map2check_nondet_float(9, 9, 1e-30f, "f");
  map2check_nondet_float(9, 9, 0.1f, "f");
  map2check_nondet_memory_byte(0, 0, 200, "__VERIFIER_nondet_memory");
  map2check_nondet_int(1, 1, 7, "a_function_name_that_is_exactly_34c");

  ASSERT_TRUE(nondet_log_to_file(map2check_nondet_get_log()));
  EXPECT_EQ(readFile("klee_log.csv"),
            "0;10;3;__VERIFIER_nondet_int;0;-1;0\n"
            "1;11;3;__VERIFIER_nondet_int;1;-2147483648;0\n"
            "2;1;2;u;2;4294967295;5\n"
            "3;5;6;ll;3;-9223372036854775808;17\n"
            "4;7;8;ull;4;18446744073709551615;18\n"
            "5;9;9;d;5;1.500000;15\n"
            "6;9;9;d;6;-0.000000;15\n"
            "7;9;9;f;7;1e-30;16\n"
            "8;9;9;f;8;0.100000001;16\n"
            "9;0;0;__VERIFIER_nondet_memory;9;200;22\n"
            "10;1;1;a_function_name_that_is_exactly_34c;10;7;0\n");
}

TEST_F(NonDetLogTest, AnEmptyLogIsAnEmptyFile) {
  ASSERT_TRUE(nondet_log_to_file(map2check_nondet_get_log()));
  EXPECT_EQ(readFile("klee_log.csv"), "");
}
