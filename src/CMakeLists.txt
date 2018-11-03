cmake_minimum_required(VERSION 3.5)

project(highermq)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-fPIC")
set(CMAKE_C_FKAGS "-fPIC")

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

include_directories(./)
link_directories(./)
aux_source_directory(. DIRSRCS)
add_executable(testbed ${DIRSRCS})
