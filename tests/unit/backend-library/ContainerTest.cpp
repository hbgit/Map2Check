#include <gtest/gtest.h>
extern "C" {
#include <modules/backend/library/header/Container.h>
}
class ContainerTest : public ::testing::Test {
 protected:
  void SetUp() override { container = new_container(HEAP_LOG_CONTAINER); }
  void TearDown() override { free_container(&container); }

  MAP2CHECK_CONTAINER container;
};

TEST_F(ContainerTest, IsEmptyInitially) { EXPECT_EQ(container.size, 0); }