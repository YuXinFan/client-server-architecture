#ifndef CS133_HW4_JOBOUTPUT_HPP_
#define CS133_HW4_JOBOUTPUT_HPP_

#include <stdlib.h>
#include <chrono>
#include "ClientID.hpp"
#include "JobID.hpp"

//JobOutput will be returned by the ServerThread and contains client-id, a job-id,
//and a float value representing the result of the scalar function evaluation
//It additionally contains a time-stamp indicating when the result has been calculated.

class JobOutput {
 public:
  JobOutput();
  JobOutput(const ClientID &clientID,
            const JobID &jobID,
            const float output,
            const std::chrono::high_resolution_clock::time_point &time);
  ClientID _clientID;
  JobID _jobID;
  float _output;
  std::chrono::high_resolution_clock::time_point _time;
};

#endif