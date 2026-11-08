#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include "../../../modules/frontend/utils/tools.hpp"

namespace {

using Map2Check::KleeLogHelper;
using Map2Check::KleeLogRow;
using Map2Check::KleeLogType;

// One klee_log.csv row per type number: id;line;scope;function;step;value;type
std::vector<KleeLogRow> parse(const std::vector<std::string>& types) {
  const std::string path = "klee_log_test.csv";
  {
    std::ofstream out(path);
    for (size_t i = 0; i < types.size(); ++i) {
      out << i << ";10;1;main;" << i << ";7;" << types[i] << "\n";
    }
  }
  std::vector<KleeLogRow> rows = KleeLogHelper::getListLogFromCSV(path);
  std::remove(path.c_str());
  return rows;
}

}  // namespace

// Every number of the runtime's enum NONDET_TYPE (Map2CheckTypes.h) has a
// type and a generator name; the parser used to know only 0-5 and left the
// type uninitialized, which the witness then read.
TEST(KleeLog, EveryRuntimeTypeHasAGeneratorName) {
  const std::vector<std::string> expected = {
      "__VERIFIER_nondet_int",       "__VERIFIER_nondet_char",
      "__VERIFIER_nondet_pointer",   "__VERIFIER_nondet_ushort",
      "__VERIFIER_nondet_long",      "__VERIFIER_nondet_uint",
      "__VERIFIER_nondet_ulong",     "__VERIFIER_nondet_bool",
      "__VERIFIER_nondet_uchar",     "__VERIFIER_nondet_pchar",
      "__VERIFIER_nondet_size_t",    "__VERIFIER_nondet_loff_t",
      "__VERIFIER_nondet_sector_t",  "__VERIFIER_nondet_uint",
      "__VERIFIER_nondet_short",     "__VERIFIER_nondet_double",
      "__VERIFIER_nondet_float",     "__VERIFIER_nondet_longlong",
      "__VERIFIER_nondet_ulonglong", "__VERIFIER_nondet_u8",
      "__VERIFIER_nondet_u16",       "__VERIFIER_nondet_u32",
      "__VERIFIER_nondet_memory"};
  std::vector<std::string> types;
  for (size_t i = 0; i < expected.size(); ++i) types.push_back(std::to_string(i));
  std::vector<KleeLogRow> rows = parse(types);
  ASSERT_EQ(rows.size(), expected.size());
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(static_cast<size_t>(rows[i].type), i) << "type " << i;
    EXPECT_EQ(rows[i].generateWitnessFunctionName(), expected[i]) << "type " << i;
  }
}

// A number outside the enum, or no number at all, is UNKNOWN with no name.
TEST(KleeLog, AnUnknownTypeIsUnknownAndHasNoName) {
  std::vector<KleeLogRow> rows = parse({"23", "-1", "abc"});
  ASSERT_EQ(rows.size(), 3u);
  for (const KleeLogRow& row : rows) {
    EXPECT_EQ(row.type, KleeLogType::UNKNOWN);
    EXPECT_EQ(row.generateWitnessFunctionName(), "");
    EXPECT_EQ(std::string(row).find("Call Function"), std::string::npos);
  }
}
