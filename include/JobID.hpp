#ifndef CS133_HW4_JOBID_HPP_
#define CS133_HW4_JOBID_HPP_

#include <stdlib.h>

class JobID {
 public:
  JobID();
  JobID(const size_t number);

  bool operator==(const JobID &rhs) const;
  bool operator<(const JobID &rhs) const;

  const size_t &number() const;

 private:
  size_t _number;
};

#endif