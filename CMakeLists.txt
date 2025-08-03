cmake_minimum_required(VERSION 3.24)

project(RayCastingGaem LANGUAGES CXX)
set(EXECUTABLE_NAME ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    #    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(CMAKE_CXX_FLAGS "/O2")
else ()
    set(CMAKE_EXE_LINKER_FLAGS "-static")
    set(CMAKE_CXX_FLAGS "-Wall -O3")
    set(CMAKE_CXX_FLAGS_RELEASE "-s")

    include(CheckIPOSupported)
    check_ipo_supported(RESULT lto_supported OUTPUT lto_error)
    if (lto_supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
        message(STATUS "LTO supported")
    else ()
        message(WARNING ${lto_error})
        message("LTO not supported, continuing without LTO...")
    endif ()
endif ()


find_package(SDL2 QUIET)

if (NOT SDL2_FOUND)
    include(FetchContent)

    if (WIN32)
        if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
            FetchContent_Declare(
                    sdl2
                    URL https://github.com/libsdl-org/SDL/releases/download/release-2.32.8/SDL2-devel-2.32.8-VC.zip
                    URL_HASH SHA256=a16e5f0e5de87a804e3c240a7d92aca0318fda176b22d95550c6512fc4f18172
            )
        else ()
            FetchContent_Declare(
                    sdl2
                    URL https://github.com/libsdl-org/SDL/releases/download/release-2.32.8/SDL2-devel-2.32.8-mingw.tar.gz
                    URL_HASH SHA256=f590b3707689562483ded05bf15eaf0f5d33eb97f49d0fd3b894225fe17cc52b
            )
        endif ()
    endif ()

    FetchContent_MakeAvailable(sdl2)

    if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        set(SDL2_INCLUDE_DIR "${sdl2_SOURCE_DIR}/include")
        set(SDL2_LIBRARY_DIR "${sdl2_SOURCE_DIR}/lib/x64")
        set(SDL2_DLL_DIR "${sdl2_SOURCE_DIR}/lib/x64")
    else ()
        set(SDL2_INCLUDE_DIR "${sdl2_SOURCE_DIR}/x86_64-w64-mingw32/include/SDL2")
        set(SDL2_LIBRARY_DIR "${sdl2_SOURCE_DIR}/x86_64-w64-mingw32/lib")
        set(SDL2_DLL_DIR "${sdl2_SOURCE_DIR}/x86_64-w64-mingw32//bin")
    endif ()
else ()
    message(STATUS "Using system-installed SDL2")
endif ()

include(FetchContent)
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
FetchContent_MakeAvailable(glm stb)

file(GLOB_RECURSE SRC_FILES
        "src/*.c"
        "src/*.cpp"
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${sdl2_SOURCE_DIR}/include
        ${glm_SOURCE_DIR}/glm
        ${stb_SOURCE_DIR}
        ${SDL2_INCLUDE_DIR}
)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_link_libraries(${PROJECT_NAME} PRIVATE
            "${SDL2_LIBRARY_DIR}/SDL2.lib"
            "${SDL2_LIBRARY_DIR}/SDL2main.lib"
    )
else ()
    target_link_libraries(${PROJECT_NAME} PRIVATE
            "${SDL2_LIBRARY_DIR}/libSDL2.dll.a"
            "${SDL2_LIBRARY_DIR}/libSDL2main.a"
            -lmingw32
    )
endif ()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_DLL_DIR}/SDL2.dll"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMENT "Copying SDL2.dll to build directory"
)

#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        "${CMAKE_SOURCE_DIR}/assets"
#        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
#)