cmake_minimum_required(VERSION 3.5)

# cmake -Dtest=ON to turn on test mode
option(test "Build all tests." OFF) 

project(eventual)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-fPIC")
set(CMAKE_C_FKAGS "-fPIC")

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

include_directories(./eventual)
link_directories(./eventual)
add_subdirectory(./eventual)

if(test STREQUAL "ON")
    add_subdirectory(googletest) 
    enable_testing()
    add_subdirectory(tests)
else()
    add_subdirectory(samples)
endif()

