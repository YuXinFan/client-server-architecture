#include <functional>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>

#include "Server.hpp"

int main() {
  typedef std::function<float(float)> scalarFct;
  //initialization of the server
  Server server(3, 10);

  //example function (only this one is used in the test)
  scalarFct sqrtFct = [](float value) {
    return std::sqrt(value);
  };

  //initialize 20 clients each one just asking the server to provide one thread
  //that calculates the square roots on demand
  std::vector<std::tuple<ClientID, Socket *> > clients;
  clients.reserve(20);
  for (int i = 0; i < 20; i++) {
    std::cout << "Trying to open session ...\n";
    std::tuple<bool, ClientID, Socket *> res = server.openSession(sqrtFct);
    if (std::get<bool>(res)) {
      std::cout << "client " << std::get<ClientID>(res).number() << " initialization succeeded\n";
      clients.emplace_back(std::get<ClientID>(res), std::get<Socket *>(res));
    }
  }

  //sleep for 10 seconds
  for (int i = 0; i < 10; i++) {
    std::cout << "sleeping...\n";
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(1000.0));
  }

  //now use the first 10 clients to submit some jobs
  for (int i = 0; i < 10; i++) {
    std::cout << "Submitting jobs from client " << std::get<ClientID>(clients[i]).number() << "\n";
    for (double v = 0.001; v < 1.0; v += 0.1) {
      JobID jobID = std::get<Socket *>(clients[i])->submitJob(std::get<ClientID>(clients[i]), v);
      std::cout << "Submitted job id " << jobID.number() << "\n";
    }
  }

  //sleep for 15 seconds
  for (int i = 0; i < 15; i++) {
    std::cout << "sleeping...\n";
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(1000.0));
  }

  //verify that all the results are there
  for (int i = 0; i < 10; i++) {
    std::cout << "Checking the results for client " << std::get<ClientID>(clients[i]).number() << "\n";

    for (int j = 0; j < 15; j++) {
      std::pair<bool, JobOutput> res = std::get<Socket *>(clients[i])->conditionalFrontAndPop_ofOutputQueue();
      if (res.first)
        std::cout << "got a result: job id " << res.second._jobID.number() << " and the value is " << res.second._output
                  << "\n";
      else
        std::cout << "failed to get another result\n";
    }

  }

  //manually close all remaining sessions
  for (int i = 0; i < 20; i++) {
    if (server.endSession(std::get<ClientID>(clients[i])))
      std::cout << "Successfully deleted thread for client " << std::get<ClientID>(clients[i]).number() << "\n";
  }

  return 0;
}