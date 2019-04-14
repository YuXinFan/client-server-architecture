template<typename T>
AtomicDequeue<T>::AtomicDequeue() : m_mutex(new std::mutex()) {}

template<typename T>
void
AtomicDequeue<T>::push_front(const T &value) {
  std::lock_guard<std::mutex> lock(*m_mutex);
  m_queue.push_front(value);
}

template<typename T>
void
AtomicDequeue<T>::push_front(T &&value) {
  std::lock_guard<std::mutex> lock(*m_mutex);
  m_queue.push_front(value);
}

template<typename T>
void
AtomicDequeue<T>::push_back(const T &value) {
  std::lock_guard<std::mutex> lock(*m_mutex);
  m_queue.push_back(value);
}

template<typename T>
void
AtomicDequeue<T>::push_back(T &&value) {
  std::lock_guard<std::mutex> lock(*m_mutex);
  m_queue.push_back(value);
}

template<typename T>
void
AtomicDequeue<T>::pop_front() {
  std::lock_guard<std::mutex> lock(*m_mutex);
  m_queue.pop_front();
}

template<typename T>
void
AtomicDequeue<T>::pop_back() {
  std::lock_guard<std::mutex> lock(*m_mutex);
  m_queue.pop_back();
}

template<typename T>
std::pair<bool, T>
AtomicDequeue<T>::conditionalFrontAndPop() {
  std::lock_guard<std::mutex> lock(*m_mutex);
  std::pair<bool, T> result;
  result.first = false;
  if (!m_queue.empty()) {
    result.first = true;
    result.second = m_queue.front();
    m_queue.pop_front();
  }
  return result;
}

template<typename T>
bool
AtomicDequeue<T>::empty() {
  std::lock_guard<std::mutex> lock(*m_mutex);
  return m_queue.empty();
}