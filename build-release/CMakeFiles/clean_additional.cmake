# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "host/CMakeFiles/pkhex-qt-host-tests_autogen.dir/AutogenUsed.txt"
  "host/CMakeFiles/pkhex-qt-host-tests_autogen.dir/ParseCache.txt"
  "host/CMakeFiles/pkhex-qt_autogen.dir/AutogenUsed.txt"
  "host/CMakeFiles/pkhex-qt_autogen.dir/ParseCache.txt"
  "host/pkhex-qt-host-tests_autogen"
  "host/pkhex-qt_autogen"
  )
endif()
