#include <chrono>

#include "gtest/gtest.h"
#include "ClientID.hpp"
#include "JobID.hpp"
#include "JobInput.hpp"
#include "JobOutput.hpp"

TEST(TestJobIO, JobInput) {
  ClientID c1{233};
  JobID j1{666};
  JobInput i2{c1, j1, 3.14f};

  EXPECT_EQ(i2._clientID.number(), 233);
  EXPECT_EQ(i2._jobID.number(), 666);
  EXPECT_FLOAT_EQ(i2._value, 3.14f);
}

TEST(TestJobIO, JobOutput) {
  ClientID c1{233};
  JobID j1{666};
  float output = 3.1415f;
  auto now = std::chrono::high_resolution_clock::now();
  JobOutput o1{c1, j1, output, now};

  EXPECT_EQ(o1._clientID.number(), c1.number());
  EXPECT_EQ(o1._jobID.number(), j1.number());
  EXPECT_FLOAT_EQ(o1._output, output);
  EXPECT_EQ(o1._time, now);
}
