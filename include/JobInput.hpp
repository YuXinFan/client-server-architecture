#ifndef CS133_HW4_JOBINPUT_HPP_
#define CS133_HW4_JOBINPUT_HPP_

#include <stdlib.h>
#include "ClientID.hpp"
#include "JobID.hpp"

//JobInput is the data-structure that will be held in the input-queue to the ServerThread.
//It contains a client-id, a job-id, and a float value for which the function should be evaluated.

class JobInput {
 public:
  JobInput();
  JobInput(const ClientID &clientID,
           const JobID &jobID,
           const float value);
  ClientID _clientID;
  JobID _jobID;
  float _value;
};

#endif