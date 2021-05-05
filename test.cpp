#include <gtest/gtest.h>

#include <ullist.hpp>

using namespace ull;

TEST(Ulllist, Init) {
  ullist<int> a;
  EXPECT_TRUE(a.empty());
  a = {0, 1, 2, 3, 4, 5, 6, 7};
  for (size_t i = 0; i < a.size(); ++i) {
    EXPECT_EQ(a[i], i);
  }

  ullist<int> b = {0, 1, 2, 3, 4, 5, 6, 7};
  for (size_t i = 0; i < b.size(); ++i) {
    EXPECT_EQ(b[i], i);
  }

  ullist<int> c(10);
  EXPECT_EQ(c.size(), 10);
  EXPECT_EQ(c.nodeCount(), 2);
}

TEST(Ullist, Resize) {
  ullist<int> a;

  a.resize(5);
  EXPECT_EQ(a.size(), 5);
  EXPECT_EQ(a.nodeCount(), 1);

  a.resize(7);
  EXPECT_EQ(a.size(), 7);
  EXPECT_EQ(a.nodeCount(), 2);

  a = {1, 2, 3, 4, 5, 6, 7};
  a.resize(3);
  EXPECT_EQ(a.size(), 3);
  EXPECT_EQ(a.nodeCount(), 1);

  a = {1, 2, 3, 4, 5};
  a.resize(7);
  EXPECT_EQ(a.size(), 7);
  EXPECT_EQ(a.nodeCount(), 2);

}

TEST(Ullist, Front) {
  ullist<int> a;
  EXPECT_ANY_THROW(a.front());

  a.resize(1);
  a.front() = 4;
  EXPECT_EQ(a.front(), 4);

  a = {0, 1, 2, 3, 4};
  EXPECT_EQ(a.front(), 0);
}

TEST(Ullist, Back) {
  ullist<int> a;
  EXPECT_ANY_THROW(a.back());

  a.resize(1);
  a.back() = 4;
  EXPECT_EQ(a.back(), 4);

  a = {0, 1, 2, 3, 4};
  EXPECT_EQ(a.back(), 4);
}

TEST(Ullist, PopBack) {
  ullist<int> a;
  EXPECT_ANY_THROW(a.pop_back());

  a = {0, 1, 2, 3, 4};
  a.pop_back();
  EXPECT_EQ(a.size(), 4);
  for (size_t i = 0; i < a.size(); ++i) {
    EXPECT_EQ(a[i], i);
  }
}

TEST(Ullist, Clear) {
  ullist<int> a;
  EXPECT_ANY_THROW(a.clear());
  a = {0, 1, 2, 3, 4};
  a.clear();
  EXPECT_TRUE(a.empty());
  EXPECT_ANY_THROW(a.front());
  EXPECT_ANY_THROW(a.back());
}

TEST(Ullist, Remove) {
  ullist<int> a;
  EXPECT_ANY_THROW(a.remove(1));
  a = {0, 1, 2, 3, 4};
  a.remove(0);
  EXPECT_EQ(a.front(), 1);
  EXPECT_EQ(a.back(), 4);

  a.remove(2);
  EXPECT_EQ(a[2], 4);
}

TEST(Ullist, Insert) {
  ullist<int> a;
  a.insert(0, 1);
  EXPECT_EQ(a.size(), 1);
  EXPECT_EQ(a.front(), 1);
  EXPECT_EQ(a.back(), 1);

  a = {1, 2, 3, 4};
  a.insert(0, 0);
  EXPECT_EQ(a.size(), 5);
  for (size_t i = 0; i < a.size(); ++i) {
    EXPECT_EQ(a[i], i);
  }
  a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  a.insert(3, 99);
  EXPECT_EQ(a[3], 99);
}
