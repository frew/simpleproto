#include "util.h"

using namespace std;

StreamableException::StreamableException() {
}

StreamableException::StreamableException(std::string description)
    : descstr(description) {
}

StreamableException::~StreamableException() throw () {
}

ostringstream& StreamableException::stream() {
  return descstr;
}

string StreamableException::description() {
  return descstr.str();
}
