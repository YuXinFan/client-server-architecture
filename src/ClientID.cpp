#include "ClientID.hpp"

ClientID::ClientID(void) { }

ClientID::ClientID(const size_t number) {
  _number = number;
}

 const size_t & ClientID::number(void) const {
   return _number;
 }

bool ClientID::operator==(const ClientID &rhs) const {
  return (number() == rhs.number());
}

bool ClientID::operator<(const ClientID &rhs) const {
  return (number() < rhs.number());
}
