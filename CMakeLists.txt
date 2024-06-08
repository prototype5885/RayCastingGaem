cmake_minimum_required(VERSION 3.28)
project(RayCastingGaem C)

set(CMAKE_C_STANDARD 99)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# add_subdirectory(external/SDL2)
# include_directories(external/SDL2/include)

find_package(SDL2 REQUIRED)

set(EXECUTABLE_NAME RayCastingGaem)

add_executable(${EXECUTABLE_NAME} src/Main.c
        src/ProToMath.c
        src/ProToMath.h)

target_link_libraries(${EXECUTABLE_NAME} -lm ${SDL2_LIBRARIES})
