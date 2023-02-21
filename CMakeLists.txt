cmake_minimum_required(VERSION 3.13)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

include(cmake/ResourceBundling.cmake)

include(FetchContent)

FetchContent_Declare(moduru
  GIT_REPOSITORY git@github.com:izzyreal/moduru.git
  GIT_TAG        master
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/editables/moduru
)

FetchContent_Declare(akaifat
  GIT_REPOSITORY git@github.com:izzyreal/akaifat.git
  GIT_TAG        main
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/editables/akaifat
)

FetchContent_Declare(ctoot
  GIT_REPOSITORY git@github.com:izzyreal/ctoot.git
  GIT_TAG        native-cmake
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/editables/ctoot
)

set(CATCH_CONFIG_NO_CPP17_UNCAUGHT_EXCEPTIONS ON)

FetchContent_Declare(Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.1.0
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/deps/Catch2
)

FetchContent_Declare(filesystem
  GIT_REPOSITORY https://github.com/gulrak/filesystem.git
  GIT_TAG        v1.5.12
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/deps/filesystem
)

FetchContent_Declare(tl-expected
  GIT_REPOSITORY https://github.com/TartanLlama/expected.git
  GIT_TAG        v1.0.0
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/deps/expected
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
)

set(RAPIDJSON_BUILD_EXAMPLES OFF)
set(RAPIDJSON_BUILD_DOC OFF)
set(RAPIDJSON_BUILD_TESTS OFF)

FetchContent_Declare(rapidjson
  GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
  GIT_TAG        v1.1.0
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/deps/rapidjson
)

set(RTMIDI_BUILD_STATIC_LIBS ON)
set(BUILD_SHARED_LIBS OFF)
set(RTMIDI_BUILD_TESTING OFF)
set(RTMIDI_TARGETNAME_UNINSTALL "rtmidi_uninstall")

FetchContent_Declare(rtmidi
    GIT_REPOSITORY https://github.com/thestk/rtmidi.git
    GIT_TAG        5.0.0
    SOURCE_DIR     ${CMAKE_SOURCE_DIR}/deps/rtmidi
)

FetchContent_MakeAvailable(rtmidi moduru akaifat ctoot filesystem rapidjson Catch2)

FetchContent_GetProperties(tl-expected)

if(NOT tl-expected_POPULATED)
  FetchContent_Populate(tl-expected)
endif()

project(mpc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-fPIC")

if(APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11" CACHE INTERNAL "Minimum OS X deployment version")
endif()

## Configure main lib ##

set(_src_root_path_main "${CMAKE_CURRENT_SOURCE_DIR}/src/main")

file(
    GLOB_RECURSE _source_list_main 
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${_src_root_path_main}/*.c*"
    "${_src_root_path_main}/*.h*"
)

foreach(_source IN ITEMS ${_source_list_main})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
    source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()

add_library(mpc ${_source_list_main})

target_include_directories(mpc PUBLIC
    ${_src_root_path_main}
    ${ctoot_SOURCE_DIR}/src/main
    ${akaifat_SOURCE_DIR}/src/main
    ${moduru_SOURCE_DIR}/src/main
    ${rapidjson_SOURCE_DIR}/include
    ${filesystem_SOURCE_DIR}/include
    ${tl-expected_SOURCE_DIR}/include
)

target_link_libraries(mpc ctoot akaifat moduru ghc_filesystem rtmidi)

if(MSVC)
	target_compile_options(mpc PRIVATE "/MP")
endif()

if(VMPC2000XL_WIN7)
    target_compile_definitions(mpc PRIVATE VMPC2000XL_WIN7)
endif()

if (IOS)
    target_compile_definitions(rtmidi PRIVATE TARGET_OS_IPHONE=1)
endif()

## Configure test suite application ##

set(_src_root_path_test "${CMAKE_CURRENT_SOURCE_DIR}/src/test")

include_directories(${_src_root_path_test})

file(
    GLOB_RECURSE _source_list_test
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${_src_root_path_test}/*.c*"
    "${_src_root_path_test}/*.h*"
)

foreach(_source IN ITEMS ${_source_list_test})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
    source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()

add_executable(mpc-tests ${_source_list_test} src/test/SamplerTest.cpp)
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

_bundle_mpc_resources(mpc)

set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES
    /.git
    /build
    /\\\\.DS_Store
)
include(CPack)
