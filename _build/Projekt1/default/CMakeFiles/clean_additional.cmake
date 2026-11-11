# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "/home/Phoenix/MPLABProjects/Projekt1/out/Projekt1/default.cmf"
  "/home/Phoenix/MPLABProjects/Projekt1/out/Projekt1/default.hex"
  "/home/Phoenix/MPLABProjects/Projekt1/out/Projekt1/default.hxl"
  "/home/Phoenix/MPLABProjects/Projekt1/out/Projekt1/default.mum"
  "/home/Phoenix/MPLABProjects/Projekt1/out/Projekt1/default.o"
  "/home/Phoenix/MPLABProjects/Projekt1/out/Projekt1/default.sdb"
  "/home/Phoenix/MPLABProjects/Projekt1/out/Projekt1/default.sym"
  )
endif()
