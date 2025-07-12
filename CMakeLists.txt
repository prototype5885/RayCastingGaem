cmake_minimum_required(VERSION 3.10)
project(RayCastingGaem)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

set(EXECUTABLE_NAME RayCastingGaem)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(WIN32)
  # set(CMAKE_C_FLAGS_RELEASE "-mwindows")
elseif(UNIX)
  # set(CMAKE_C_FLAGS_RELEASE "")
endif()

set(CMAKE_C_FLAGS "-Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -s")

# set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ffast-math -march=native")

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

file(GLOB_RECURSE SRC_FILES
  "src/*.c"
  "src/*.cpp"
)

add_executable(${EXECUTABLE_NAME} ${SRC_FILES})

if(WIN32)
  target_link_libraries(${EXECUTABLE_NAME} PRIVATE -lmingw32 -lSDL2main -lSDL2 -flto)
elseif(UNIX)
  target_link_libraries(${EXECUTABLE_NAME} PRIVATE -lSDL2main -lSDL2 -flto)
endif()
