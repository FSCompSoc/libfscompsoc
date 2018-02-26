#pragma once

#include <cstring>
#include <exception>

#define FSCOMPSOC_MAKE_EXCEPTION(EXCEPTION_NAME, DEFAULT_MESSAGE)              \
  class EXCEPTION_NAME : public std::exception                                 \
  {                                                                            \
  private:                                                                     \
    const char* msg;                                                           \
                                                                               \
  public:                                                                      \
    const char* what() const noexcept override { return msg; }                 \
                                                                               \
  public:                                                                      \
    EXCEPTION_NAME() : msg(DEFAULT_MESSAGE) {}                                 \
    EXCEPTION_NAME(const char* msg) : msg(msg) {}                              \
  };

namespace fscompsoc::exceptions
{
  /// An exception that should not ever happen, but does not necessarily require
  /// an abort()
  class InternalException : std::exception
  {
  public:
    std::exception e;
    const char* base;

  public:
    const char* what() const noexcept override
    {
      const char* msg = e.what();
      char* ret = static_cast<char*>(calloc(strlen(base) + strlen(msg), 1));

      strcpy(ret, base);
      strcat(ret, msg);
      return ret;
    }

  public:
    InternalException(std::exception e)
        : e(e), base("An unexpected error occured: ")
    {
    }
    InternalException(std::exception e, const char* msg) : e(e), base(msg) {}
  };

  FSCOMPSOC_MAKE_EXCEPTION(ParseFailed, "Parsing failed");

  FSCOMPSOC_MAKE_EXCEPTION(InvalidArgument, "The argument was invalid");

  FSCOMPSOC_MAKE_EXCEPTION(Unsupported, "An unsupported action was requested");

  FSCOMPSOC_MAKE_EXCEPTION(NotDoneYet, "The function reached incomplete code");

  FSCOMPSOC_MAKE_EXCEPTION(InvalidOperation,
                           "An invalid operation was requested");

  FSCOMPSOC_MAKE_EXCEPTION(TimedOut, "The operation timed out");

  // An exception that should never be caught, as it represents a fundemental
  // inconsitency in the program
  FSCOMPSOC_MAKE_EXCEPTION(Insane,
                           "The operation encountered unexpected behaviour")
} // namespace fscompsoc::exceptions
