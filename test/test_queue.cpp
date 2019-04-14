#include <vector>
#include <thread>
#include <random>
#include <set>

#include "gtest/gtest.h"
#include "AtomicDequeue.hpp"

using std::vector;
using std::set;
using std::thread;

const int N{100};

class QueueTest : public ::testing::Test {
 protected:
  void SetUp() override {
    v1.reserve(N);
    v2.reserve(N);
    for (int i{0}; i < 100; ++i) {
      v1.push_back(n(e));
      v2.push_back(n(e));
    }
    s1.insert(v1.cbegin(), v1.cend());
    s1.insert(v2.cbegin(), v2.cend());
  }

  static std::default_random_engine e;
  static std::normal_distribution<double> n;
  vector<double> v1, v2;
  set<double> s1;
};

std::default_random_engine QueueTest::e(19260817);
std::normal_distribution<double> QueueTest::n;

TEST_F(QueueTest, Mutex) {
  AtomicDequeue<double> q1;
  thread t1{[&]() {
    for (const auto &v : v1) {
      q1.push_front(v);
    }
  }};
  thread t2{[&]() {
    for (const auto &v : v2) {
      q1.push_back(v);
    }
  }};
  t1.join();
  t2.join();

  int count{0};
  while (true) {
    auto r = q1.conditionalFrontAndPop();
    if (!r.first) {
      break;
    } else {
      count++;
      auto v = r.second;
      EXPECT_NE(s1.find(v), s1.cend());
      ASSERT_LT(count, N * 2 + 1);
    }
  }
  EXPECT_EQ(count, N * 2);
  EXPECT_EQ(q1.empty(), true);
}
