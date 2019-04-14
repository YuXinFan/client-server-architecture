#ifndef CS133_ATOMICDEQUEUE_HPP_
#define CS133_ATOMICDEQUEUE_HPP_

#include <stdlib.h>
#include <queue>
#include <memory>
#include <mutex>

//template, thread-safe double-ended queue
//each of the member functions makes use of the class-attached mutex to make the operations on the member m_queue mutually exclusive

template<typename T>
class AtomicDequeue {
 public:
  AtomicDequeue();

  void push_front(const T &value);
  void push_front(T &&value);
  void push_back(const T &value);
  void push_back(T &&value);
  void pop_front();
  void pop_back();

  //checking whether or not the queue contains data.
  //If yes, return a pair<bool,T> where T is the front element of the queue, and bool is set to true to indicate success.
  //If not, the bool should be false.
  //The front element should furthermore be popped from the queue before returning (if available).
  std::pair<bool, T> conditionalFrontAndPop();
  bool empty();

 private:
  std::deque<T> m_queue;
  std::shared_ptr<std::mutex> m_mutex;
};

#include "AtomicDequeue_impl.hpp"

#endif