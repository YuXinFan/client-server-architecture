#include "JobID.hpp"

JobID::JobID(void) { }

JobID::JobID(const size_t number) {
  _number = number;
}

 const size_t & JobID::number(void) const {
   return _number;
 }

bool JobID::operator==(const JobID &rhs) const {
  return (number() == rhs.number());
}

bool JobID::operator<(const JobID &rhs) const {
  return (number() < rhs.number());
}
