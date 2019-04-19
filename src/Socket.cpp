#include "Socket.hpp"
#include <iostream>

Socket::Socket() {
  // Init counter variables
  _load = 0;
  _jobCounter = 1;
 }

JobID Socket::submitJob(const ClientID &clientID, const float value) {
  JobID job_id = JobID(_jobCounter++);
  _inputQueue.push_back(std::move(JobInput(clientID, job_id, value)));
  return job_id;
}

std::pair<bool, JobInput> Socket::conditionalFrontAndPop_ofInputQueue(void) {
  return (_inputQueue.conditionalFrontAndPop());
}

std::pair<bool, JobOutput> Socket::conditionalFrontAndPop_ofOutputQueue(void) {
  return (_outputQueue.conditionalFrontAndPop());
}

void Socket::putBackToInputQueue(const JobInput &input) {
  _inputQueue.push_front(input);
}

void Socket::submitResult(const ClientID &clientID, 
                          const JobID &jobID, 
                          const float result,
                          const std::chrono::high_resolution_clock::time_point &lastBusyTime) {
  _outputQueue.push_back(JobOutput(clientID, jobID, result, lastBusyTime));
}

size_t & Socket::load(void) {
  return _load;
}

std::pair<bool, JobOutput> Socket::getResult(const JobID &i) {
  // Fetch all JobOutputs in socket to output buffer
  std::pair<bool, JobOutput>  pop_of_output = conditionalFrontAndPop_ofOutputQueue();
  while ( pop_of_output.first ) {
    _outputBuffer[pop_of_output.second._jobID] = pop_of_output.second;
    pop_of_output = conditionalFrontAndPop_ofOutputQueue();
  }

  std::pair<bool, JobOutput> result;
  result.first = false;
  // Find and return the corresponding Joboutput
  if (_outputBuffer.count(i) > 0) {
    result.first = true;
    result.second = _outputBuffer[i];
  }
  return result;
}