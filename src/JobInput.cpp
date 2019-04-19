#include "JobInput.hpp"

JobInput::JobInput() { }

JobInput::JobInput(const ClientID &clientID, const JobID &jobID, const float value) {
  _clientID = clientID;
  _jobID = jobID;
  _value = value;
}