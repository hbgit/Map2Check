#include <gtest/gtest.h>
extern "C" {
#include <modules/backend/library/header/AnalysisMode.h>
void map2check_malloc(void *ptr, int size);
void map2check_calloc(void *ptr, int quantity, int size);
void map2check_free_resolved_address(void *ptr, unsigned line,
                                     const char *function_name,
                                     short int isNullValid);

int __map2check_main__() { return 0; }
}

class MemoryTrackTest : public ::testing::Test {
 protected:
  void SetUp() override { analysis_init(); }
  void TearDown() override { analysis_destroy(); }
};

TEST_F(MemoryTrackTest, DefaultIsOk) {
  Bool expectedValue = TRUE;
  Bool actualValue = analysis_is_program_correct();
  EXPECT_EQ(actualValue, expectedValue);
}

TEST_F(MemoryTrackTest, MallocShouldGenerateMemleak) {
  int random_address;
  map2check_malloc(&random_address, 32);
  Bool expectedValue = FALSE;
  Bool actualValue = analysis_is_program_correct();
  EXPECT_EQ(actualValue, expectedValue);
}

TEST_F(MemoryTrackTest, CallocShouldGenerateMemleak) {
  int random_address;
  map2check_calloc(&random_address, 10, 32);
  Bool expectedValue = FALSE;
  Bool actualValue = analysis_is_program_correct();
  EXPECT_EQ(actualValue, expectedValue);
}

TEST_F(MemoryTrackTest, AfterReleaseMalloc) {
  int random_address;
  map2check_malloc(&random_address, 32);
  map2check_free_resolved_address(&random_address, 0, "some_var", 0);
  Bool expectedValue = TRUE;
  Bool actualValue = analysis_is_program_correct();
  EXPECT_EQ(actualValue, expectedValue);
}

TEST_F(MemoryTrackTest, AfterReleaseCalloc) {
  int random_address;
  map2check_calloc(&random_address, 10, 32);
  map2check_free_resolved_address(&random_address, 0, "some_var", 0);
  Bool expectedValue = TRUE;
  Bool actualValue = analysis_is_program_correct();
  EXPECT_EQ(actualValue, expectedValue);
}