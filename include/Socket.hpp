#ifndef CS133_SOCKET_HPP_
#define CS133_SOCKET_HPP_

#include <chrono>
#include <thread>
#include <map>
#include "AtomicDequeue.hpp"
#include "JobInput.hpp"
#include "JobOutput.hpp"

typedef AtomicDequeue<JobInput> InputQueue;
typedef AtomicDequeue<JobOutput> OutputQueue;

class Socket {
 public:
  Socket();

  //submit a job to the back of the input queue. Takes the client-id and the float value
  //for which a function should be evaluated, and returns the JobID assigned to this
  //job (using jobCounter).
  JobID submitJob(const ClientID &clientID, const float value);
  //interface-forward of the conditionalFrontAndPop function of the input queue
  std::pair<bool, JobInput> conditionalFrontAndPop_ofInputQueue();
  //to put a job back to the front of the input queue. This is used internally if a thread took
  //a job but it turns out that it is from a wrong client and was meant for another thread
  // operating on the same socket
  void putBackToInputQueue(const JobInput &input);
  //interface-forward of the conditionalFrontAndPop function of the output queue
  std::pair<bool, JobOutput> conditionalFrontAndPop_ofOutputQueue();
  //used internally to push results back to the output-queue. Parameters are self-explanatory
  void submitResult(const ClientID &clientID,
                    const JobID &jobID,
                    const float result,
                    const std::chrono::high_resolution_clock::time_point &lastBusyTime);
  //used to get the load
  size_t &load();
  // query with explict job id. Since each socket could be shared among
  // several clients, client could get other clients' results if no JobID
  // checking is performed. Note that you could fetch all available results to
  // temporary buffers (_outputBuffer) before querying JobID.
  std::pair<bool, JobOutput> getResult(const JobID &i);

 private:
  InputQueue _inputQueue;
  OutputQueue _outputQueue;
  std::map<JobID, JobOutput> _outputBuffer;
  size_t _load;             //indicates the number of threads operating on this queue
  size_t _jobCounter;       //increments everytime a new job is sent to the input-queue
};

#endif