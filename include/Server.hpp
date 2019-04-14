#ifndef CS133_SERVER_HPP_
#define CS133_SERVER_HPP_

#include <thread>
#include <memory>
#include <map>
#include <vector>
#include "Socket.hpp"
#include "ServerThread.hpp"
#include "JobInput.hpp"
#include "JobOutput.hpp"
#include "ClientID.hpp"
#include "JobID.hpp"

class Server {
 public:
  //Constructor: initializes the vector of sockets (remains fixed afterwards)
  //initializes the clientCounter to 1
  //starts the watchdog thread
  Server(const size_t maximumClientsPerSocket = 3,
         const size_t numberSockets = 10);
  //makes sure to join the watchdog thread
  virtual ~Server();

  //Main function from the front-end called by a client to get a new server-thread
  //Simply takes a reference to the scalar function to be executed by ServerThread,
  //and returns a tuple indicating whether or not the operation has been a success,
  //the new issued client ID, and a pointer to the socket over which the requests will run
  //Note that besides starting the thread openSession also needs to check the load
  //on the sockets and choose the one with minimum load. If all sockets are at full capacity
  //the operation should fail. Use mutex to keep operations safe
  std::tuple<bool, ClientID, Socket *> openSession(const std::function<float(float)> &fct);
  //Find the thread assigned to a clientID, and stop it
  //Don’t forget to adjust the load
  //the function returns true if a thread has been correctly stopped
  bool endSession(const ClientID &clientID);

  //BONUS: watchdog keeps iterating through the threadPool in regular intervals, and removes threads
  //that have done nothing for 20s. Do not forget to adjust the socket-load
  void watchDog(bool &serverRunning,
                std::map<ClientID, std::shared_ptr<ServerThread> > &threadPool,
                std::mutex &threadPoolMutex,
                std::vector<Socket> &sockets);

 private:
  unsigned int _maximumClientsPerSocket;
  std::vector<Socket> _sockets;
  bool _serverRunning;

  int _clientCounter;       //counts clients and increments everytime a new ClientID is issued
  std::map<ClientID, std::shared_ptr<ServerThread> > _threadPool;
  std::mutex _threadPoolMutex;

  std::thread _watchDogThread;
};

#endif