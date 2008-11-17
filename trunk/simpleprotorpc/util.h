#ifndef _SIMPLEPROTORPC_UTIL_H_
#define _SIMPLEPROTORPC_UTIL_H_

#include <sstream>

/**
 * A StreamableException allows the programmer to specify
 * the exception's description via a stream interface.
 */
class StreamableException {
 public:
  StreamableException();
  explicit StreamableException(std::string description);
  StreamableException(const StreamableException& rhs);
  virtual ~StreamableException() throw ();
  virtual std::ostringstream& stream();
  virtual std::string description() const;
 protected:
  std::ostringstream descstr;
};

#endif
