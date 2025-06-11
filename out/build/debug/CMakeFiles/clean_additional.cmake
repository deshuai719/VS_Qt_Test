# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\MNIC_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\MNIC_autogen.dir\\ParseCache.txt"
  "MNIC_autogen"
  )
endif()
