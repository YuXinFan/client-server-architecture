#ifndef CS133_SERVERTHREAD_HPP_
#define CS133_SERVERTHREAD_HPP_

#include <chrono>
#include <thread>
#include <functional>
#include "ClientID.hpp"
#include "Socket.hpp"

class ServerThread {
 public:
  //Constructor: takes ClientID, pointer to the socket it operates over, and a reference
  //to a scalar function that is to be evaluated inside the thread for each incoming job
  ServerThread(const ClientID clientID,
               Socket *const socket,
               const std::function<float(float)> &fct);
  //Destructor: needs to stop thread!
  virtual ~ServerThread();

  //the function running in the thread. Takes the parameters passed to the constructor,
  //and further references to _running, _lastBusyTime, and _lastBusyTimeMutex. Note that
  //the function needs to check whether or not the input on the inputQueue has the correct client
  //ID. If not, those inputs should be pushed back to the front of the queue! The function should
  //furthermore safely update lastBusyTime each time a new job is processed
  void serverThread(const std::function<float(float)> &fct,
                    const ClientID clientID,
                    Socket *const socket,
                    bool &running,
                    std::chrono::high_resolution_clock::time_point &lastBusyTime,
                    std::mutex &lastBusyTimeMutex);

  //simple getter
  Socket *socket();
  //simple getter
  ClientID clientID();
  //simple getter (creation of return value may not be atmic, use mutex!)
  std::chrono::high_resolution_clock::time_point getLastBusyTime();
  //set lastBusyTiem (use mutex!)
  void setLastBusyTime(const std::chrono::high_resolution_clock::time_point &lastBusyTime);

 private:
  ClientID _clientID;
  Socket *_socket;
  bool _running;        //referred to by thread to indicate whether or not it should keep running
  std::thread _thread;

  std::chrono::high_resolution_clock::time_point _lastBusyTime; //updated each time a new job has been processed
  std::mutex _lastBusyTimeMutex;                                //ensures thread-saftey on the time_point
};

#endif