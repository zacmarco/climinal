cmake_policy(SET CMP0053 OLD)

IF(NOT EXISTS "@CMAKE_CURRENT_BINARY_DIR@/install_manifest.txt")
  MESSAGE(FATAL_ERROR "Cannot find install manifest: " @CMAKE_CURRENT_BINARY_DIR@/install_manifest.txt "")
ENDIF(NOT EXISTS "@CMAKE_CURRENT_BINARY_DIR@/install_manifest.txt")

FILE(READ "@CMAKE_CURRENT_BINARY_DIR@/install_manifest.txt" files)
STRING(REGEX REPLACE " " ";" files "${files}")
STRING(REGEX REPLACE "\n" ";" files "${files}")
FOREACH(file ${files})
  MESSAGE(STATUS "Uninstalling " $ENV{DESTDIR}${file} "")
  IF(EXISTS "$ENV{DESTDIR}${file}")
    EXEC_PROGRAM(
      "@CMAKE_COMMAND@" ARGS "-E remove " $ENV{DESTDIR}${file} ""
      OUTPUT_VARIABLE rm_out
      RETURN_VALUE rm_retval
      )
    IF(NOT "${rm_retval}" STREQUAL 0)
      MESSAGE(STATUS "Problem when removing " $ENV{DESTDIR}${file} "")
    ENDIF(NOT "${rm_retval}" STREQUAL 0)
  ELSE(EXISTS "$ENV{DESTDIR}${file}")
    MESSAGE(STATUS "File " $ENV{DESTDIR}${file} " does not exist.")
  ENDIF(EXISTS "$ENV{DESTDIR}${file}")
ENDFOREACH(file)

#MESSAGE(STATUS "Uninstalling " $ENV{DESTDIR} "@CMAKE_INSTALL_PREFIX@/include/climinal")
#EXEC_PROGRAM(
#  "@CMAKE_COMMAND@" ARGS "-E remove " $ENV{DESTDIR} "@CMAKE_INSTALL_PREFIX@/include/climinal"
#  OUTPUT_VARIABLE rm_out
#  RETURN_VALUE rm_retval
#  )
#IF(NOT "${rm_retval}" STREQUAL 0)
#  MESSAGE(FATAL_ERROR "Problem when removing " $ENV{DESTDIR} "@CMAKE_INSTALL_PREFIX@/include/climinal")
#ENDIF(NOT "${rm_retval}" STREQUAL 0)

