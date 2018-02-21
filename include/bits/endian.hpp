#pragma once

namespace fscompsoc::bits {
  enum Endianness {
    BigEndian,
    LittleEndian
  };

  constexpr Endianness NetworkEndian = Endianness::BigEndian;

  #ifdef __BIG_ENDIAN__
    #define __FSCOMPSOC__BIG__ENDIAN__
    constexpr Endianness HostEndian = Endianness::BigEndian;
  #elif defined(__LITTLE_ENDIAN__) || defined(_WIN32)
    #define __FSCOMPSOC__LITTLE__ENDIAN__
    constexpr Endianness HostEndian = Endianness::LittleEndian;
  #else
    #pragma warn("Cannot determine endianness, defaulting to little endian")
    #define __FSCOMPSOC__LITTLE__ENDIAN__
  #endif

  template<Endianness Endian, typename T>
  T to_host_endian(T t);

  template<Endianness Endian, typename T>
  T from_host_endian(T t);

  template<typename T>
  T b2l_endian(T t);

  template<typename T>
  T l2b_endian(T t);

  // Yes I know about the byte order fallacy, but this is easier

  template<typename T>
  inline T to_host_endian<BigEndian, T>(T t) {
    #ifdef __FSCOMPSOC__BIG__ENDIAN__
      return t;
    #else
      return b2l_endian(t);
    #endif
  }

  template<typename T>
  inline T to_host_endian<LittleEndian, T>(T t) {
    #ifdef __FSCOMPSOC__LITTLE__ENDIAN__
      return t;
    #else
      return l2b_endian(t);
    #endif
  }

  template<typename T>
  inline T from_host_endian<BigEndian, T>(T t) {
    #ifdef __FSCOMPSOC__BIG__ENDIAN__
    return t;
    #else
    return l2b_endian(t);
    #endif
  }

  template<typename T>
  inline T from_host_endian<LittleEndian, T>(T t) {
    #ifdef __FSCOMPSOC__LITTLE__ENDIAN__
      return t;
    #else
      return b2l_endian(t);
    #endif
  }
}
