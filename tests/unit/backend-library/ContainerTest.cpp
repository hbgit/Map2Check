#include <gtest/gtest.h>
extern "C" {
#include <modules/backend/library/header/AllocationLog.h>
#include <modules/backend/library/header/Container.h>
}

namespace {
void add_item(MAP2CHECK_CONTAINER *c, long addr) {
  MEMORY_ALLOCATIONS_ROW row = new_memory_row(addr, FALSE);
  append_element(c, &row);
}

long get_item(MAP2CHECK_CONTAINER *c, unsigned key) {
  MEMORY_ALLOCATIONS_ROW *row =
      (MEMORY_ALLOCATIONS_ROW *)get_element_at(key, *c);
  return row->addr;
}
}  // namespace
class ContainerTest : public ::testing::Test {
 protected:
  void SetUp() override { container = new_container(ALLOCATION_LOG_CONTAINER); }
  void TearDown() override { free_container(&container); }

  MAP2CHECK_CONTAINER container;
};

TEST_F(ContainerTest, IsEmptyInitially) { EXPECT_EQ(container.size, 0); }

TEST_F(ContainerTest, AddItemShouldIncreaseSize) {
  add_item(&container, 0);
  EXPECT_EQ(container.size, 1);
}

TEST_F(ContainerTest, AddedItemShouldBeSetted) {
  long expectedValue = 42;
  add_item(&container, expectedValue);
  long actualValue = get_item(&container, 0);
  EXPECT_EQ(actualValue, expectedValue);
}

TEST_F(ContainerTest, MultipleItemsAddedShouldNotBreak) {
  int numberOfItems = 10000;

  for (int i = 0; i < numberOfItems; i++) {
    add_item(&container, numberOfItems - i);
  }

  for (int i = 0; i < numberOfItems; i++) {
    long expectedValue = numberOfItems - i;
    long actualValue = get_item(&container, i);
    EXPECT_EQ(actualValue, expectedValue);
  }
}
