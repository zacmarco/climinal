# Specify the minimum version for CMake

cmake_minimum_required(VERSION 2.8)

# Project's name
project(climinal-tools)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-g")

# In case Boost is installed, we can use it to enable the advanced parser based on boost
find_package(Boost 1.58.0)
if(Boost_FOUND)
  message("Boost library found, using next gen parser")
  include_directories(${Boost_INCLUDE_DIRS})
  add_compile_options(-DINCLUDE_BOOST)
  #set(CMAKE_CXX_FLAGS $CMAKE_INSTALL_PREFIX" -DINCLUDE_BOOST_DEV")
endif()


# Now make sure that you the the build directory on your "Include" path when compiling
include_directories(${PROJECT_SOURCE_DIR})

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB CLIMINALBUILDER_SOURCES "climinalbuilder/*.cpp")
file(GLOB CLIMINALBUILDER_HEADERS "climinalbuilder/*.h")

#Generate the shared library from the sources
add_executable(climinalbuilder ${CLIMINALBUILDER_SOURCES})

add_custom_target(parser)
add_custom_command(TARGET parser
        COMMAND cd ../climinalbuilder
        COMMAND flex -o ../climinalbuilder/lex.yy.cpp ../climinalbuilder/climinalbuilder.l
        COMMAND bison -d -o ../climinalbuilder/y.tab.cpp ../climinalbuilder/climinalbuilder.y
)
 
#Set the location for library installation -- i.e., /usr/lib in this case
# not really necessary in this example. Use "sudo make install" to apply
install(TARGETS climinalbuilder DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

#Debian Package generator
SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Marco Zaccheria")
SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Climinal tools")

SET(CPACK_PACKAGE_NAME "climinal-tools")
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
SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")

INCLUDE(CPack)

########### Add uninstall target ###############
CONFIGURE_FILE(
  "${CMAKE_CURRENT_BINARY_DIR}/../cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/../cmake_uninstall.cmake")
