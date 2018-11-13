#include <gtest/gtest.h>
extern "C" {
#include <modules/backend/library/header/Container.h>
#include <modules/backend/library/header/HeapLog.h>
}

namespace {
MEMORY_HEAP_ROW create_row(void *addr, int size, int primitive_size) {
  return new_heap_row(0, 0, addr, size, primitive_size, "test");
}
}  // namespace

class HeapLogTest : public ::testing::Test {
 protected:
  void SetUp() override { container = new_container(HEAP_LOG_CONTAINER); }
  void TearDown() override { free_container(&container); }

  MAP2CHECK_CONTAINER container;
};

TEST_F(HeapLogTest, MarkHeapShouldIncreaseSize) {
  int var;
  MEMORY_HEAP_ROW row = create_row(&var, 4, 4);
  mark_heap_log(&container, row);
  EXPECT_EQ(container.size, 1);
}

TEST_F(HeapLogTest, ValidPrimitive) {
  unsigned size = sizeof(int);
  int var;
  MEMORY_HEAP_ROW row = create_row(&var, size, size);
  mark_heap_log(&container, row);
  Bool actual = is_valid_heap_address(&container, &var, size);
  EXPECT_EQ(actual, TRUE);
}

TEST_F(HeapLogTest, InvalidPrimitive) {
  unsigned size = sizeof(int);
  int var;
  int another_var;
  MEMORY_HEAP_ROW row = create_row(&var, size, size);
  mark_heap_log(&container, row);
  Bool actual = is_valid_heap_address(&container, &another_var, size);
  EXPECT_EQ(actual, FALSE);
}

TEST_F(HeapLogTest, ValidArray) {
  unsigned size = sizeof(int);
  int var[2];
  char *ptr = (char *)var;
  ptr += size;
  MEMORY_HEAP_ROW row = create_row(&var, size * 2, size);
  mark_heap_log(&container, row);
  Bool actual = is_valid_heap_address(&container, (void *)ptr, size);
  EXPECT_EQ(actual, TRUE);
}

TEST_F(HeapLogTest, InvalidArray) {
  unsigned size = sizeof(int);
  int var[2];
  char *ptr = (char *)var;
  ptr += size;
  ptr += size;
  MEMORY_HEAP_ROW row = create_row(&var, size * 2, size);
  mark_heap_log(&container, row);
  Bool actual = is_valid_heap_address(&container, (void *)ptr, size);
  EXPECT_EQ(actual, FALSE);
}