cmake_minimum_required(VERSION 3.13)

include(cmake/CPM.cmake)
include(cmake/ResourceBundling.cmake)

set(MPC_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE STRING "Setting MPC_SOURCE_DIR")
set(MPC_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE STRING "Setting MPC_BINARY_DIR")

add_definitions(-w)


CPMAddPackage("gh:izzyreal/moduru#no-conan")
CPMAddPackage("gh:izzyreal/ctoot#no-conan")
CPMAddPackage("gh:izzyreal/akaifat#no-conan")
CPMAddPackage("gh:Tencent/rapidjson#v1.1.0")
CPMAddPackage("gh:gulrak/filesystem#v1.5.12")

CPMAddPackage("gh:catchorg/Catch2#v3.1.0")

project(mpc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-fPIC -DCATCH_CONFIG_NO_CPP17_UNCAUGHT_EXCEPTIONS")

if(DEFINED ENV{APPLE_M1})
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE INTERNAL "")
endif()

if(APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11" CACHE INTERNAL "Minimum OS X deployment version")
endif()

## Configure main lib ##

set(_src_root_path_main "${CMAKE_CURRENT_SOURCE_DIR}/src/main")

include_directories(${_src_root_path_main} ${CTOOT_SOURCE_DIR}/src/main ${AKAIFAT_SOURCE_DIR}/src/main ${MODURU_SOURCE_DIR}/src/main ${rapidjson_SOURCE_DIR} ${expected_INCLUDE_DIR})
link_directories(${CTOOT_BINARY_DIR} ${AKAIFAT_BINARY_DIR} ${MODURU_BINARY_DIR})

file(
    GLOB_RECURSE _source_list_main 
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${_src_root_path_main}/*.c*"
    "${_src_root_path_main}/*.cpp*"
    "${_src_root_path_main}/*.hpp*"
	"${_src_root_path_main}/*.hxx*"
)

foreach(_source IN ITEMS ${_source_list_main})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
    source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()

add_library(mpc ${_source_list_main})
target_link_libraries(mpc ctoot akaifat moduru ghc_filesystem)

_bundle_mpc_resources(mpc)

if(MSVC)
	target_compile_options(mpc PRIVATE "/MP")
endif()

if(VMPC2000XL_WIN7)
    target_compile_definitions(mpc PRIVATE VMPC2000XL_WIN7)
endif()

## Configure test suite application ##

set(_src_root_path_test "${CMAKE_CURRENT_SOURCE_DIR}/src/test")

include_directories(${_src_root_path_test})

file(
    GLOB_RECURSE _source_list_test
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${_src_root_path_test}/*.c*"
    "${_src_root_path_test}/*.cpp*"
    "${_src_root_path_test}/*.hpp*"
)

foreach(_source IN ITEMS ${_source_list_test})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
    source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()

add_executable(mpc-tests ${_source_list_test})
target_link_libraries(mpc-tests mpc Catch2::Catch2WithMain)

if (UNIX)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(mpc-tests Threads::Threads)
endif (UNIX)

if (UNIX AND NOT APPLE)
    include(FindPkgConfig)
    pkg_search_module(udisks2 REQUIRED udisks2)
    target_include_directories(mpc SYSTEM PUBLIC ${udisks2_INCLUDE_DIRS})
    target_include_directories(mpc-tests SYSTEM PUBLIC ${udisks2_INCLUDE_DIRS})
    target_link_libraries(mpc-tests ${udisks2_LIBRARIES})
    target_link_libraries(mpc ${udisks2_LIBRARIES})
endif()

if (APPLE)
    target_link_libraries(mpc-tests stdc++ "-framework Foundation -framework Security -framework DiskArbitration -framework SystemConfiguration")
endif()
