# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Weater-App_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Weater-App_autogen.dir\\ParseCache.txt"
  "Weater-App_autogen"
  )
endif()
