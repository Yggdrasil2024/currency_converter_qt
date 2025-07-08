# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\currency_converter2i_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\currency_converter2i_autogen.dir\\ParseCache.txt"
  "currency_converter2i_autogen"
  )
endif()
