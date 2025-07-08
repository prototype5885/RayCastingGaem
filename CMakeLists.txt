cmake_minimum_required(VERSION 3.25)
project(RayCastingGaem)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(EXECUTABLE_NAME RayCastingGaem)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math -march=native")

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

file(GLOB_RECURSE SRC_FILES
    "src/*.c"
    "src/*.cpp"
    "src/*.h"
    "src/*.hpp"
)

add_executable(${EXECUTABLE_NAME} ${SRC_FILES})

if(WIN32)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE -lmingw32 -lSDL2main -lSDL2 -static-libstdc++)
elseif(UNIX)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE  -lSDL2main -lSDL2 -static-libstdc++)
endif()
