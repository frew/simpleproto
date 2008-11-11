#ifndef _UTIL_H_
#define _UTIL_H_

#include <sstream>

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

/**
 * A StreamableException allows the programmer to specify
 * the exception's description via a stream interface.
 */
class StreamableException : public std::exception {
 public:
  StreamableException();
  explicit StreamableException(std::string description);
  virtual ~StreamableException() throw ();
  virtual std::ostringstream& stream();
  virtual std::string description();
 protected:
  std::ostringstream descstr;
 private:
  DISALLOW_COPY_AND_ASSIGN(StreamableException);
};

#endif
