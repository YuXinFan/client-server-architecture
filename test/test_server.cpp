#include <algorithm>
#include <map>
#include <future>
#include <thread>
#include <chrono>
#include <functional>
#include <tuple>

#include "gtest/gtest.h"
#include "Server.hpp"

typedef std::function<float(float)> scalarFct;

class TestServer : public ::testing::Test {
 protected:
  const size_t N_CLIENTS_PER_SOCKET{3}, N_SOCKETS{10}, N_JOBS{20},
      N_INVALID_FETCH{5};
  Server server{N_CLIENTS_PER_SOCKET, N_SOCKETS};
};

TEST_F(TestServer, test_open_session) {
  scalarFct pow2 = [](float x_) -> float { return x_ * x_; };

  size_t maximum_load = 0;
  for (int i{0}; i < N_CLIENTS_PER_SOCKET * N_SOCKETS; ++i) {
    auto ret = server.openSession(pow2);
    EXPECT_TRUE(std::get<0>(ret));
    if (0 == i) {
      EXPECT_EQ(std::get<1>(ret), 1);
    }
    size_t current_load = std::get<2>(ret)->load();
    EXPECT_LE(current_load, N_CLIENTS_PER_SOCKET);
    EXPECT_LE(current_load - maximum_load, 1) << "load balance?";
    maximum_load = std::max(maximum_load, current_load);
  }

  auto ret = server.openSession(pow2);
  EXPECT_FALSE(std::get<0>(ret));
}

TEST_F(TestServer, test_close_session) {
  scalarFct pow2 = [](float x_) -> float { return x_ * x_; };
  std::map<ClientID, Socket *> socket_pool;

  for (int i{0}; i < N_CLIENTS_PER_SOCKET * N_SOCKETS; ++i) {
    auto ret = server.openSession(pow2);
    socket_pool.insert({std::get<1>(ret), std::get<2>(ret)});
  }

  for (auto &s : socket_pool) {
    ClientID i{s.first};
    size_t c1 = s.second->load();
    EXPECT_TRUE(server.endSession(i));
    EXPECT_EQ(s.second->load(), c1 - 1);
  }

  for (auto &s : socket_pool) {
    ClientID i{s.first};
    EXPECT_FALSE(server.endSession(i));
  }
}

TEST_F(TestServer, test_submit_job) {
  using namespace std::chrono_literals;

  scalarFct pow2 = [](float x_) -> float { return x_ * x_; };
  const float val{3.14f};
  std::map<ClientID, Socket *> socket_pool;
  std::map<ClientID, std::vector<JobID>> job_id_pool;

  for (int i{0}; i < N_CLIENTS_PER_SOCKET * N_SOCKETS; ++i) {
    auto ret = server.openSession(pow2);
    EXPECT_TRUE(std::get<0>(ret));
    socket_pool.insert({std::get<1>(ret), std::get<2>(ret)});
  }

  std::this_thread::sleep_for(1s);

  for (auto &c : socket_pool) {
    ClientID c_id{std::get<0>(c)};
    for (size_t j{0}; j < N_JOBS; ++j) {
      auto j_id = std::get<1>(c)->submitJob(c_id, val * j);
      job_id_pool[c_id].push_back(j_id);
    }
  }

  std::this_thread::sleep_for(1s);

  for (auto &c : socket_pool) {
    for (size_t j{0}; j < N_JOBS + N_INVALID_FETCH; ++j) {
      if (j < N_JOBS) {
        auto j_id = job_id_pool[std::get<0>(c)][j];
        auto ret = std::get<1>(c)->getResult(j_id);
        EXPECT_TRUE(std::get<0>(ret));
        EXPECT_FLOAT_EQ(std::get<1>(ret)._output, pow2(val * j));
      } else {
        auto j_id = job_id_pool[std::get<0>(c)][N_JOBS];
        JobID dummy_jid(j_id.number() + 233);
        auto ret = std::get<1>(c)->getResult(dummy_jid);
        EXPECT_FALSE(std::get<0>(ret));
      }
    }
  }
}

TEST_F(TestServer, test_watch_dog) {
  using namespace std::chrono_literals;

  // We launch a set of dummy threads, keeping only part of them busy, to test
  // whether `watchDog` work properly. The job launching logic here does not
  // depend or related to `openSession` method.
  scalarFct pow2 = [](float x_) -> float { return x_ * x_; };
  bool is_runnig{true};
  std::map<ClientID, std::shared_ptr<ServerThread>> thread_pool;
  std::mutex m;
  std::vector<Socket> socket_pool(1);
  const size_t n_workers = 5, n_active_workers = 3;

  for (size_t i{0}; i < n_workers; ++i) {
    ClientID c{i};
    auto t = std::make_shared<ServerThread>(c, &(socket_pool[0]), pow2);
    thread_pool.insert({c, t});
    socket_pool[0].load()++;
  }

  std::thread t_watch_dog{&Server::watchDog, &server,
                          std::ref(is_runnig), std::ref(thread_pool), std::ref(m), std::ref(socket_pool)};

  // watch dog thread is 20 seconds...
  std::this_thread::sleep_for(15s);
  EXPECT_EQ(socket_pool[0].load(), n_workers);

  for (size_t i{0}; i < n_active_workers; ++i) {
    ClientID c{i};
    float x{2.33333f};
    auto s = socket_pool[0].submitJob(c, x);
  }

  std::this_thread::sleep_for(10s);
  EXPECT_EQ(socket_pool[0].load(), n_active_workers);
  EXPECT_EQ(thread_pool.size(), n_active_workers);
  for (size_t i{0}; i < n_workers; ++i) {
    ClientID c{i};
    if (i < n_active_workers) {
      EXPECT_NE(thread_pool.find(c), thread_pool.cend());
      thread_pool.erase(c);
    } else {
      EXPECT_EQ(thread_pool.find(c), thread_pool.cend());
    }
  }

  is_runnig = false;
  t_watch_dog.join();
}
