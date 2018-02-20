cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(FSRouterPlanner)


if ( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

include_directories(include)

add_subdirectory(src)
