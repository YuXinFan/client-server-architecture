#include <vector>
#include <random>
#include <thread>
#include <deque>

#include "gtest/gtest.h"
#include "Socket.hpp"
#include "ClientID.hpp"
#include "JobID.hpp"

TEST(SocketTest, JobOps) {
  std::default_random_engine e{19260817};
  std::normal_distribution<float> n;
  const int N{10000};
  std::vector<float> inputs;

  inputs.reserve(N);
  for (int i{0}; i < N; ++i) {
    inputs.push_back(n(e));
  }

  ClientID c1{0};
  Socket s1;

  auto i1 = s1.conditionalFrontAndPop_ofInputQueue();
  EXPECT_FALSE(i1.first);

  auto j1 = s1.submitJob(c1, inputs[0]);
  EXPECT_EQ(j1.number(), 1);

  // t1: put `inputs` to the end of s1._inputQueue
  // t2: keeping popping the inputQueue, then put inputs beck to queue
  // the order of inputs should not be changed
  std::thread t1{[&]() {
    for (auto i{inputs.cbegin() + 1}; i != inputs.cend(); ++i) {
      s1.submitJob(c1, *i);
    }
  }};
  std::thread t2{[&]() {
    std::deque<JobInput> stk;
    while (stk.size() < N) {
      auto r = s1.conditionalFrontAndPop_ofInputQueue();
      if (r.first) {
        stk.push_back(r.second);
      }
    }
    while (!stk.empty()) {
      s1.putBackToInputQueue(stk.back());
      stk.pop_back();
    }
  }};
  t1.join();
  t2.join();

  for (const auto &v : inputs) {
    auto i = s1.conditionalFrontAndPop_ofInputQueue();
    EXPECT_TRUE(i.first);
    EXPECT_FLOAT_EQ(i.second._value, v);
  }

  // TODO: cover remaining APIs?
}
