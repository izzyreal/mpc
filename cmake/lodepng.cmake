# cmake/lodepng.cmake
#
# Minimal CMake integration for lodepng.
# Downloads only lodepng.cpp and lodepng.h (no full repo).

set(lodepng_SOURCE_DIR ${CMAKE_SOURCE_DIR}/deps/lodepng)
file(MAKE_DIRECTORY ${lodepng_SOURCE_DIR})

set(_lodepng_cpp ${lodepng_SOURCE_DIR}/lodepng.cpp)
set(_lodepng_h   ${lodepng_SOURCE_DIR}/lodepng.h)

if(NOT EXISTS ${_lodepng_cpp})
    message(STATUS "Downloading lodepng.cpp...")
    file(DOWNLOAD
        https://raw.githubusercontent.com/lvandeve/lodepng/master/lodepng.cpp
        ${_lodepng_cpp}
    )
endif()

if(NOT EXISTS ${_lodepng_h})
    message(STATUS "Downloading lodepng.h...")
    file(DOWNLOAD
        https://raw.githubusercontent.com/lvandeve/lodepng/master/lodepng.h
        ${_lodepng_h}
    )
endif()

add_library(lodepng
    ${_lodepng_cpp}
    ${_lodepng_h}
)

target_include_directories(lodepng PUBLIC ${lodepng_SOURCE_DIR})
