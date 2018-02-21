#include "bits/endian.hpp"

#include <cstdint>

#define FSCOMPSOC_L2B(TYPE, ARG)                                               \
  template<>                                                                   \
  TYPE l2b_endian<TYPE>(TYPE ARG)

#define FSCOMPSOC_B2L(TYPE, ARG)                                               \
  template<>                                                                   \
  TYPE b2l_endian<TYPE>(TYPE ARG)

namespace fscompsoc::bits {
  FSCOMPSOC_L2B(uint8_t, i) { return i; }
  FSCOMPSOC_B2L(uint8_t, i) { return i; }
  
}
