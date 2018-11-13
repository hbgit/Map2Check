#include <gtest/gtest.h>
extern "C" {
#include <modules/backend/library/lib/BTree.h>
}

class BTreeTest : public ::testing::Test {
 protected:
  void SetUp() override {
    system("rm -rf btree_test.bin");
    bt = B_TREE_CREATE("btree_test.bin");
  }
  //   void TearDown() override { B_TREE_FREE(&bt); }
  B_TREE bt;
};

TEST_F(BTreeTest, ConstructorShouldBeSettedRight) {
  EXPECT_STREQ(bt.filename, "btree_test.bin");
}

TEST_F(BTreeTest, AddItem) {
  B_TREE_ROW row;
  row.index = 0;
  B_TREE_INSERT(&bt, &row);
}

TEST_F(BTreeTest, AddItemShouldSetValue) {
  B_TREE_ROW row;
  row.index = 0;
  B_TREE_INSERT(&bt, &row);

  int actual = B_TREE_SEARCH(&bt, 0) == NULL ? 0 : 1;
  int expected = 1;

  EXPECT_EQ(actual, expected);
}

TEST_F(BTreeTest, AddMultipleItems) {
  unsigned length = 10000;
  B_TREE_ROW row[length];
  for (int i = 0; i < length; i++) {
    row[i].index = i + 1;
    ;
    B_TREE_INSERT(&bt, &row[i]);
  }

  for (int i = 0; i < length; i++) {
    B_TREE_ROW* result = B_TREE_SEARCH(&bt, i + 1);
    ASSERT_FALSE(result == NULL);
    unsigned actual = result->index;
    unsigned expected = i + 1;

    EXPECT_EQ(actual, expected);
  }
}