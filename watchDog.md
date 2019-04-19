## WatchDog Design
1. Create and start watchDog thread with serverRunning status `true`, then watchDog keep running.
2. Exit watchDog by modifying reference variable `serverRunning` to `false` and join thread.
3. While watchDog running, he traversal the map `threadPool`. For each thread he checked, calculate the time difference between now and its last busy time. If the time difference is larger than 20 seconds, watchDog erases the thread from the threadPool and adjusts the corresponding socket load.
4. After one traverse, watchDog wait some time, which set as 10ms in my implementation, then start a new traverse.

## WatchDog Implementation
```C++
void Server::watchDog(bool &serverRunning,
                      std::map<ClientID, std::shared_ptr<ServerThread> > &threadPool,
                      std::mutex &threadPoolMutex,
                      std::vector<Socket> &sockets) {
  while ( serverRunning == true ) {
    // Traversal the map
    for ( auto it = threadPool.begin(); it != threadPool.end(); it++ ) {
      std::chrono::duration<double> diff_time =std::chrono::high_resolution_clock::now()- it->second->getLastBusyTime();
      // Check the time diff between now and lastbusytime
      if ( diff_time.count() > 20.f ) {
        threadPoolMutex.lock();
        threadPool[it->first]->socket()->load()--;
        threadPool.erase(it->first);
        threadPoolMutex.unlock();
      }
    }
    // After a traverse, wait some time
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}
```