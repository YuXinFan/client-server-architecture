#ifndef CS133_HW4_CLIENTID_HPP_
#define CS133_HW4_CLIENTID_HPP_

#include <stdlib.h>

class ClientID {
 public:
  ClientID();
  ClientID(const size_t number);

  bool operator==(const ClientID &rhs) const;
  bool operator<(const ClientID &rhs) const;

  const size_t &number() const;

 private:
  size_t _number;
};

#endif