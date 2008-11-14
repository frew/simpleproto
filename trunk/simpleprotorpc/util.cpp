#include "util.h"

using namespace std;

StreamableException::StreamableException() {
}

StreamableException::StreamableException(std::string description)
    : descstr(description) {
}

StreamableException::StreamableException(const StreamableException& rhs)
    : descstr(rhs.description()) {
}

StreamableException::~StreamableException() throw () {
}

ostringstream& StreamableException::stream() {
  return descstr;
}

string StreamableException::description() const {
  return descstr.str();
}
