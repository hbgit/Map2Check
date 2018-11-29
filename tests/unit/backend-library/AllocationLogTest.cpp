#include <gtest/gtest.h>
extern "C" {
#include <modules/backend/library/header/AllocationLog.h>
#include <modules/backend/library/header/Container.h>
}

class AllocationLogTest : public ::testing::Test {
 protected:
  void SetUp() override { container = new_container(ALLOCATION_LOG_CONTAINER); }
  void TearDown() override { free_container(&container); }

  MAP2CHECK_CONTAINER container;
};

TEST_F(AllocationLogTest, MarkAllocShouldIncreaseSize) {
  mark_allocation_log(&container, 42);
  EXPECT_EQ(container.size, 1);
}

TEST_F(AllocationLogTest, MarkDeallocShouldIncreaseSize) {
  mark_deallocation_log(&container, 42);
  EXPECT_EQ(container.size, 1);
}

TEST_F(AllocationLogTest, MemoryAddressStatusCaseDynamic) {
  mark_allocation_log(&container, 42);
  enum MemoryAddressStatus expected = DYNAMIC;
  enum MemoryAddressStatus actual =
      check_address_allocation_log(&container, 42);
  EXPECT_EQ(actual, expected);
}

TEST_F(AllocationLogTest, MemoryAddressStatusCaseFree) {
  mark_deallocation_log(&container, 42);
  enum MemoryAddressStatus expected = FREE;
  enum MemoryAddressStatus actual =
      check_address_allocation_log(&container, 42);
  EXPECT_EQ(actual, expected);
}

TEST_F(AllocationLogTest, MemoryAddressStatusCaseEmpty) {
  enum MemoryAddressStatus expected = STATIC;
  enum MemoryAddressStatus actual =
      check_address_allocation_log(&container, 72);
  EXPECT_EQ(actual, expected);
}

TEST_F(AllocationLogTest, MemoryAddressStatusCaseStatic) {
  mark_allocation_log(&container, 42);
  enum MemoryAddressStatus expected = STATIC;
  enum MemoryAddressStatus actual =
      check_address_allocation_log(&container, 72);
  EXPECT_EQ(actual, expected);
}

TEST_F(AllocationLogTest, IfAddressIsReleasedThenAllocationLogIsValid) {
  mark_allocation_log(&container, 42);
  mark_deallocation_log(&container, 42);
  Bool actual = valid_allocation_log(&container);
  EXPECT_EQ(actual, TRUE);
}

TEST_F(AllocationLogTest, IfAddressIsNotReleasedThenAllocationLogIsInvalid) {
  // Map2check releases all found memleaks, so we need to alloc it
  int *a = (int *)malloc(sizeof(int));
  mark_allocation_log(&container, (long)a);
  Bool actual = valid_allocation_log(&container);
  EXPECT_EQ(actual, FALSE);
}

TEST_F(AllocationLogTest, AddressIsInAllocationLog) {
  int *ptr = (int *)malloc(sizeof(int));
  unsigned size = sizeof(int);
  MEMORY_ALLOCATIONS_ROW *row =
      (MEMORY_ALLOCATIONS_ROW *)malloc(sizeof(MEMORY_ALLOCATIONS_ROW));
  *row = new_memory_row((long)ptr, FALSE);
  row->size = size;
  append_element(&container, row);
  long actual = is_valid_allocation_address(&container, ptr, 4);
  EXPECT_NE(actual, 0);
}

TEST_F(AllocationLogTest, AddressIsNotInAllocationLog) {
  int *ptr = (int *)malloc(sizeof(int));
  unsigned size = sizeof(int);
  MEMORY_ALLOCATIONS_ROW *row =
      (MEMORY_ALLOCATIONS_ROW *)malloc(sizeof(MEMORY_ALLOCATIONS_ROW));
  *row = new_memory_row((long)ptr, FALSE);
  row->size = size;
  append_element(&container, row);
  long actual = is_valid_allocation_address(&container, ptr, size * 2);
  EXPECT_EQ(actual, 0);
}