#include <future>
#include <iostream>
#include <thread>
#include <chrono>

#include "ServerThread.hpp"

ServerThread::ServerThread(const ClientID clientID,
                           Socket *const socket,
                           const std::function<float(float)> &fct) {
  _clientID = clientID;
  _socket = socket;
  _running = true;
  _lastBusyTime = std::chrono::high_resolution_clock::now();
  _thread = std::thread(&ServerThread::serverThread, this, fct, clientID, socket, std::ref(_running), std::ref(_lastBusyTime), std::ref(_lastBusyTimeMutex));
}

ServerThread::~ServerThread(void) {
  std::cout << "~ServerThread"<<std::endl;
  _running = false;
  _thread.join();
  
}

void ServerThread::serverThread(const std::function<float(float)> &fct,
                                const ClientID clientID,
                                Socket *const socket,
                                bool &running,
                                std::chrono::high_resolution_clock::time_point &lastBusyTime,
                                std::mutex &lastBusyTimeMutex) {
  while ( running == true ){
    std::this_thread::sleep_for(std::chrono::nanoseconds(500));
    std::pair<bool, JobInput> job_in = socket->conditionalFrontAndPop_ofInputQueue();
    if (job_in.first == true){
      if ( job_in.second._clientID == clientID ) {     
        socket->submitResult(std::move(clientID), std::move(job_in.second._jobID), fct(job_in.second._value), lastBusyTime);
        lastBusyTimeMutex.lock();    
        lastBusyTime = std::chrono::high_resolution_clock::now();
        lastBusyTimeMutex.unlock();
      } else {   
        socket->putBackToInputQueue(job_in.second);
      }
    }
  }
  std::cout <<"serverThread:break"<<std::endl;
}

Socket *ServerThread::socket(void) {
  return _socket;
}

ClientID ServerThread::clientID(void) {
  return _clientID;
}

std::chrono::high_resolution_clock::time_point ServerThread::getLastBusyTime(void) {
  return _lastBusyTime;
}

void ServerThread::setLastBusyTime(const std::chrono::high_resolution_clock::time_point &lastBusyTime) {
  _lastBusyTime = lastBusyTime;
}