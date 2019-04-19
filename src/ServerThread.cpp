#include "ServerThread.hpp"

ServerThread::ServerThread(const ClientID clientID,
                           Socket *const socket,
                           const std::function<float(float)> &fct) {
  _clientID = clientID;
  _socket = socket;
  // Init running status and assign lastbusytime as start time
  _running = true;
  _lastBusyTime = std::chrono::high_resolution_clock::now();
  // Create thread for working
  _thread = std::thread(&ServerThread::serverThread, this, fct, clientID, socket, std::ref(_running), std::ref(_lastBusyTime), std::ref(_lastBusyTimeMutex));
}

ServerThread::~ServerThread(void) {
  // Set running status to false, and join thread
  _running = false;
  _thread.join();
}

void ServerThread::serverThread(const std::function<float(float)> &fct,
                                const ClientID clientID,
                                Socket *const socket,
                                bool &running,
                                std::chrono::high_resolution_clock::time_point &lastBusyTime,
                                std::mutex &lastBusyTimeMutex) {
  // While status is running, keep running
  while ( running == true ){
    std::pair<bool, JobInput> job_in = socket->conditionalFrontAndPop_ofInputQueue();
    // Check pop succeed
    if (job_in.first == true){
      // Check whether clientID matched
      if ( job_in.second._clientID == clientID ) {     
        socket->submitResult(std::move(clientID), std::move(job_in.second._jobID), fct(job_in.second._value), lastBusyTime);
        lastBusyTimeMutex.lock();    
        lastBusyTime = std::chrono::high_resolution_clock::now();
        lastBusyTimeMutex.unlock();
      } else {   
        // Push back unmatched Job
        socket->putBackToInputQueue(job_in.second);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
    }else{
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }
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
  _lastBusyTimeMutex.lock();
  _lastBusyTime = lastBusyTime;
  _lastBusyTimeMutex.unlock();
}