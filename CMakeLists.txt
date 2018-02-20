cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(libfscompsoc)

include(InstallRequiredSystemLibraries)

if ( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

include_directories(include)

file(GLOB_RECURSE source src/*.cpp)

add_library(fscompsoc SHARED ${source})

target_link_libraries(fscompsoc)

install(
  TARGETS fscompsoc
  LIBRARY DESTINATION lib
)

install(
  DIRECTORY include/
  DESTINATION include/fscompsoc
)

install(FILES cmake/libfscompsoc-config.cmake DESTINATION /usr/lib/cmake/libfscompsoc)

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "FSCompSoc")
SET(CPACK_PACKAGE_VERSION "0.0.1")

if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()

INCLUDE(CPack)

add_custom_target(default ALL)
