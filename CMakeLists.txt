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
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

find_package(SDL2 REQUIRED)

add_executable(${EXECUTABLE_NAME} 
        src/Main.cpp
        src/ProToMath.cpp
        src/ProToMath.h
        src/TextureLoader.cpp
        src/TextureLoader.h
        )

target_link_libraries(${EXECUTABLE_NAME} -lm ${SDL2_LIBRARIES})
