cmake_minimum_required(VERSION 2.8)
project(mpc)

set(_src_root_path "${CMAKE_CURRENT_SOURCE_DIR}/src")

get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)

include_directories( ${_src_root_path} )
include_directories( ${PARENT_DIR}/moduru/src )
include_directories( ${PARENT_DIR}/ctoot/src )
include_directories( ${PARENT_DIR}/rapidjson/include )

file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${_src_root_path}/*.cpp*"
    "${_src_root_path}/*.hpp*"
)

if(APPLE)
  set(CMAKE_OSX_ARCHITECTURES "x86_64")
  set(CMAKE_CXX_STANDARD 17)
endif(APPLE)

if(UNIX AND NOT APPLE)
        set(LINUX TRUE)
endif()

if (LINUX)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

add_library(mpc ${_source_list})

#if(WIN32)
#	target_compile_options(mpc PRIVATE "/MP")
#	if (BUILD_DYNAMIC)
#		target_compile_options(mpc PRIVATE "/MD$<$<CONFIG:Debug>:d>")
#	else()
#		target_compile_options(mpc PRIVATE "/MT$<$<CONFIG:Debug>:d>")
#	endif(BUILD_DYNAMIC)
#endif(WIN32)

foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
    source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()
