#include <iostream>

#include "Server.hpp"

Server::Server(const size_t maximuxClientsPerSocket, 
               const size_t numberSockets ) {
  _maximumClientsPerSocket = maximuxClientsPerSocket;
  _clientCounter = 1;
  _serverRunning = true;
  _sockets = std::vector<Socket>(numberSockets, Socket());
  _watchDogThread = std::thread(&Server::watchDog, this, std::ref(_serverRunning), std::ref(_threadPool), std::ref(_threadPoolMutex), std::ref(_sockets));

}

Server::~Server() { 
  _serverRunning = false;
  _watchDogThread.join();
}

std::tuple<bool, ClientID, Socket *> Server::openSession(const std::function<float(float)> &fct) {
  std::tuple<bool, ClientID, Socket *> result = std::make_tuple(false, ClientID(_clientCounter), nullptr);
  _clientCounter++; 
  for ( int i = 0; i < _sockets.size(); i++) {
    if ( std::get<2>(result) != nullptr ) {
      if ( _sockets[i].load() < std::get<2>(result)->load() ) {
        std::get<2>(result) = &_sockets[i] ;
      }
    }else {
      std::get<2>(result) = &_sockets[i];
    }
  }
  
  if ( std::get<2>(result) != nullptr ) {
    _threadPoolMutex.lock();
    if ( std::get<2>(result)->load() < _maximumClientsPerSocket ) { 
      std::get<0>(result) = true;
      _threadPool[std::get<1>(result)] = std::make_shared<ServerThread>(std::ref(std::get<1>(result)), std::ref(std::get<2>(result)), fct);
      std::get<2>(result)->load()+=1;
      
    }
    _threadPoolMutex.unlock();
  }
  return result;
}

bool Server::endSession(const ClientID &clientID) {
  bool result = false;
  if ( _threadPool.count(clientID) > 0 ) {
    _threadPoolMutex.lock();
    _threadPool[clientID]->socket()->load()--;
    _threadPool.erase(clientID);
    _threadPoolMutex.unlock();
    result = true;
  }
  return result;
}

void Server::watchDog(bool &serverRunning,
                      std::map<ClientID, std::shared_ptr<ServerThread> > &threadPool,
                      std::mutex &threadPoolMutex,
                      std::vector<Socket> &sockets) {
  while ( serverRunning == true ) {
    std::cout<<"watchDog:threadPool:size"<<_threadPool.size()<<std::endl;
    for ( auto it = threadPool.begin(); it != threadPool.end(); it++ ) {
      std::chrono::duration<double> diff_time =std::chrono::high_resolution_clock::now()- it->second->getLastBusyTime();
      std::cout << "watchDog: "<<diff_time.count()<<std::endl;
      if ( diff_time.count() > 20.f ) {
        threadPoolMutex.lock();
        threadPool[it->first]->socket()->load()--;
        threadPool.erase(it->first);
        threadPoolMutex.unlock();
      }
    }
    std::this_thread::sleep_for(std::chrono::microseconds(1000));
  }
}