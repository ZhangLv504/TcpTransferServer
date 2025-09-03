# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\TcpTransferServer_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\TcpTransferServer_autogen.dir\\ParseCache.txt"
  "TcpTransferServer_autogen"
  )
endif()
