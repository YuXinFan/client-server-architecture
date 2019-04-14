#include "gtest/gtest.h"
#include "ClientID.hpp"
#include "JobID.hpp"

TEST(TestID, ClientID) {
  ClientID i1{0}, i2{1}, i3{2}, i4{2};

  EXPECT_EQ(i3, i4);
  EXPECT_LT(i1, i2);
  EXPECT_LT(i2, i3);
  EXPECT_EQ(i3.number(), 2);
}

TEST(TestID, JobID) {
  JobID i1{0}, i2{1}, i3{2}, i4{2};

  EXPECT_EQ(i1.number(), 0);
  EXPECT_EQ(i2.number(), 1);
  EXPECT_EQ(i3.number(), 2);
  EXPECT_EQ(i4.number(), 2);
}
