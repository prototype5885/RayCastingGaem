cmake_minimum_required(VERSION 3.14)

project(RayCastingGaem)
set(EXECUTABLE_NAME ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "-Wall -O3")
    set(CMAKE_CXX_FLAGS_RELEASE "-s")
    #    if (CMAKE_BUILD_TYPE STREQUAL "Release")
    #        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    #    endif ()
    #    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffast-math")
    #    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")

elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "/O2")
endif ()


set(SDL2_STATIC ON)

include(FetchContent)
FetchContent_Declare(
        sdl
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.32.8
)
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
)
FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG master
)
FetchContent_MakeAvailable(sdl glm stb)


file(GLOB_RECURSE SRC_FILES
        "src/*.c"
        "src/*.cpp"
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE -static SDL2::SDL2-static)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${glm_SOURCE_DIR}
        ${stb_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
)