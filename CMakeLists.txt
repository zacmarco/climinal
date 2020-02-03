# Specify the minimum version for CMake

cmake_minimum_required(VERSION 2.8)

INCLUDE (CheckIncludeFiles)

# Project's name

project(climinal)

set(CMAKE_BUILD_TYPE Release)

CHECK_INCLUDE_FILES(termios.h HAVE_TERMIOS_H)
CHECK_INCLUDE_FILES(sys/ioctl.h HAVE_SYS_IOCTL_H)

IF(HAVE_TERMIOS_H)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DHAVE_TERMIOS_H")
ENDIF(HAVE_TERMIOS_H)
    
IF(HAVE_SYS_IOCTL_H)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DHAVE_SYS_IOCTL_H")
ENDIF(HAVE_SYS_IOCTL_H)

# Enable this for debug symbols
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -ggdb")

#configure_file ("${PROJECT_SOURCE_DIR}/cmake_config.h.in"
#                "${PROJECT_SOURCE_DIR}/config.h" )


# Now make sure that you the the build directory on your "Include" path when compiling
include_directories(${PROJECT_SOURCE_DIR})

include_directories(include)
 
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "src/*.c")
file(GLOB HEADERS "include/climinal/*.h")

#Generate the shared library from the sources
add_library(climinal SHARED ${SOURCES})
 
#Set the location for library installation -- i.e., /usr/lib in this case
# not really necessary in this example. Use "sudo make install" to apply
install(TARGETS climinal DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/climinal)

#Debian Package generator
SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Marco Zaccheria")
SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "CLI generator and library")

SET(CPACK_PACKAGE_NAME "libcliminal")
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "0")

# Architecture: (mandatory)
IF(NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)
  # There is no such thing as i686 architecture on debian, you should use i386 instead
  # $ dpkg --print-architecture
  FIND_PROGRAM(DPKG_CMD dpkg)
  IF(NOT DPKG_CMD)
    MESSAGE(STATUS "Can not find dpkg in your path, default to i386.")
    SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386)
  ENDIF(NOT DPKG_CMD)
  EXECUTE_PROCESS(COMMAND "${DPKG_CMD}" --print-architecture
    OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
ENDIF(NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)

SET(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}-dev")

INCLUDE(CPack)

########### Add uninstall target ###############
CONFIGURE_FILE(
  "${CMAKE_CURRENT_BINARY_DIR}/../cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/../cmake_uninstall.cmake")
