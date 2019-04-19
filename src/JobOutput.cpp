#include "JobOutput.hpp"

JobOutput::JobOutput() {}

JobOutput::JobOutput(const ClientID &clientID,
                     const JobID &jobID,
                     const float output,
                     const std::chrono::high_resolution_clock::time_point &time) {
  _clientID = clientID;
  _jobID = jobID;
  _output = output;
  _time = time;          
}
