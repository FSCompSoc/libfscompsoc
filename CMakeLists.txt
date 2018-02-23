cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

execute_process(COMMAND git describe --tags --long
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_VER
)

project(libfscompsoc)

include(InstallRequiredSystemLibraries)

if ( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

include_directories(include)

file(GLOB_RECURSE source src/*.cpp)

if (UNIX)
  file(GLOB_RECURSE plat_source src/*.unix.cxx)
  set(source ${source} ${plat_source})
  message(${source})
endif()
if (LINUX)
  file(GLOB_RECURSE plat_source src/*.linux.cxx)
  set(source ${source} ${plat_source})
endif()
if (APPLE)
  file(GLOB_RECURSE plat_source src/*.osx.cxx)
  set(source ${source} ${plat_source})
endif()
if (WIN32)
  file(GLOB_RECURSE plat_source src/*.win.cxx)
  set(source ${source} ${plat_source})
endif()

add_library(fscompsoc SHARED ${source})

target_link_libraries(fscompsoc)

install(
  TARGETS fscompsoc
  LIBRARY DESTINATION lib
  COMPONENT lib
)

install(
  DIRECTORY include/fscompsoc
  DESTINATION include/
  COMPONENT dev
)

install(
  FILES cmake/libfscompsoc-config.cmake
  DESTINATION /usr/lib/cmake/libfscompsoc
  COMPONENT cmake
)

SET(CPACK_PACKAGE_VERSION ${GIT_VER})

if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()

if(NOT DEFINED CPACK_GENERATOR)
  # TODO: platform logic
  set(CPACK_GENERATOR "DEB")
endif()

if(CPACK_GENERATOR STREQUAL "DEB")
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "FSCompSoc")
  set(CPACK_DEB_COMPONENT_INSTALL ON)
  set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
  set(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS ON)
  set(CPACK_DEBIAN_LIB_PACKAGE_NAME "libfscompsoc")
endif()


set(CPACK_COMPONENT_DEV_DEPENDS lib)
set(CPACK_COMPONENT_CMAKE_DEPENDS dev)

INCLUDE(CPack)

#add_custom_target(rebuild ALL DEPENDS fscompsoc)
