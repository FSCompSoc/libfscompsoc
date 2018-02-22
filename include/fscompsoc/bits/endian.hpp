#pragma once

#include <cstdint>
#include <algorithm>
#include <array>

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

  template<typename T>
  T b2l_endian(T t) {
    auto arr = reinterpret_cast<std::array<uint8_t, sizeof(T)>*>(&t);
    std::reverse(arr->begin(), arr->end());
    return t;
  }

  template<typename T>
  T l2b_endian(T t) {
    auto arr = reinterpret_cast<std::array<uint8_t, sizeof(T)>*>(&t);
    std::reverse(arr->begin(), arr->end());
    return t;
  }

  // Yes I know about the byte order fallacy, but this is easier

  template<typename T>
  inline T b2h_endian(T t) {
    #ifdef __FSCOMPSOC__BIG__ENDIAN__
      return t;
    #else
      return b2l_endian(t);
    #endif
  }

  template<typename T>
  inline T l2h_endian(T t) {
    #ifdef __FSCOMPSOC__LITTLE__ENDIAN__
      return t;
    #else
      return l2b_endian(t);
    #endif
  }

  template<typename T>
  inline T h2b_endian(T t) {
    #ifdef __FSCOMPSOC__BIG__ENDIAN__
    return t;
    #else
    return l2b_endian(t);
    #endif
  }

  template<typename T>
  inline T h2l_endian(T t) {
    #ifdef __FSCOMPSOC__LITTLE__ENDIAN__
      return t;
    #else
      return b2l_endian(t);
    #endif
  }

  template<Endianness Endian, typename T>
  class endian {
  public:
    T data;

  public:
    template<Endianness Target>
    operator endian<Target, T>() const;

    operator T() const;

  public:
    // Converts from host endian to Endian endian
    endian(T);

  public:
    inline endian<Endian, T> wrap(T t) { return reinterpret_cast<endian<Endian, T>> (t);}
  };

  template<typename T>
  class endian<LittleEndian, T> {
  public:
    T data;

  public:
    inline operator endian<LittleEndian, T>() const { return *this; };
    inline operator endian<BigEndian, T>() const {
      return endian<BigEndian, T>::wrap(l2b_endian(data));
    };

    inline operator T() const { return l2h_endian(data); }

  public:
    inline endian(T t) : data(h2l_endian(t)) {}
  };

  template<typename T>
  class endian<BigEndian, T> {
  public:
    T data;

  public:
    inline operator endian<BigEndian, T>() const { return *this; };
    inline operator endian<LittleEndian, T>() const {
      return endian<LittleEndian, T>::wrap(b2l_endian(data));
    };

    inline operator T() const { return b2h_endian(data); }

  public:
    inline endian(T t) : data(h2b_endian(t)) {}
  };
}
