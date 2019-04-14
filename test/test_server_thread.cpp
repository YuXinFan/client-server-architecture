#include <future>
#include <thread>
#include <chrono>
#include <functional>

#include "gtest/gtest.h"
#include "ServerThread.hpp"

typedef std::function<float(float)> scalarFct;
typedef std::chrono::high_resolution_clock::time_point timePoint;

class TestServerThread : public ::testing::Test {
 protected:
  ClientID c1{233};
  JobID j1{1};
  Socket s1;
  bool running{true};
  timePoint tp1;
  std::mutex m;
  scalarFct pow2 = [](float x_) -> float { return x_ * x_; };
  float x{2.33333f};
  int num_jobs{13};
};

TEST_F(TestServerThread, thread_func) {
  using namespace std::chrono_literals;

  auto future = std::async(std::launch::async, &ServerThread::serverThread, static_cast<ServerThread *>(nullptr),
                           std::ref(pow2), c1, &s1, std::ref(running), std::ref(tp1), std::ref(m));

  // test with valid task
  for (int i{0}; i < num_jobs; ++i) {
    s1.submitJob(c1, x * i);
  }
  std::this_thread::sleep_for(1s);
  for (int i{0}; i < num_jobs; ++i) {
    auto ret_output = s1.conditionalFrontAndPop_ofOutputQueue();
    EXPECT_TRUE(ret_output.first);
    EXPECT_FLOAT_EQ(ret_output.second._output, pow2(x * i)) << "at iter: " << i;
  }
  auto ret_output = s1.conditionalFrontAndPop_ofOutputQueue();
  EXPECT_FALSE(ret_output.first);

  // test with invalid task (from different ClientID)
  ClientID c2{234};
  s1.submitJob(c2, x);
  std::this_thread::sleep_for(1s);
  ret_output = s1.conditionalFrontAndPop_ofOutputQueue();
  EXPECT_FALSE(ret_output.first);
  auto ret_input = s1.conditionalFrontAndPop_ofInputQueue();
  EXPECT_TRUE(ret_input.first);
  EXPECT_EQ(ret_input.second._clientID, c2);
  EXPECT_FLOAT_EQ(ret_input.second._value, x);

  auto status = future.wait_for(1s);
  EXPECT_NE(status, std::future_status::ready);

  running = false;
  status = future.wait_for(1s);
  EXPECT_EQ(status, std::future_status::ready);

  // TODO: how to test whether setter and getter of `_LastBusyTime` are protected by mutex?
}
