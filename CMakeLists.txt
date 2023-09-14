cmake_minimum_required(VERSION 3.13)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(HAVE_STDBOOL_H ON)

include(cmake/ResourceBundling.cmake)

include(FetchContent)

FetchContent_Declare(akaifat
  GIT_REPOSITORY https://github.com/izzyreal/akaifat.git
  GIT_TAG        wasm 
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/editables/akaifat
)

set(CATCH_CONFIG_NO_CPP17_UNCAUGHT_EXCEPTIONS ON)

FetchContent_Declare(Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.3.2
  GIT_SHALLOW    ON
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/deps/Catch2
)

FetchContent_Declare(filesystem
  GIT_REPOSITORY https://github.com/gulrak/filesystem.git
  GIT_TAG        v1.5.12
  GIT_SHALLOW    ON
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/deps/filesystem
)

FetchContent_Declare(tl-expected
  GIT_REPOSITORY https://github.com/TartanLlama/expected.git
  GIT_TAG        v1.0.0
  GIT_SHALLOW    ON
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
  GIT_SHALLOW    ON
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/deps/rapidjson
)

set(RTMIDI_BUILD_STATIC_LIBS ON)
set(BUILD_SHARED_LIBS OFF)
set(RTMIDI_BUILD_TESTING OFF)
set(RTMIDI_TARGETNAME_UNINSTALL "rtmidi_uninstall")

FetchContent_Declare(rtmidi
    GIT_REPOSITORY https://github.com/thestk/rtmidi.git
    GIT_TAG        master
    GIT_SHALLOW    ON
    SOURCE_DIR     ${CMAKE_SOURCE_DIR}/deps/rtmidi
)

set(BUILD_TESTING OFF)

FetchContent_Declare(libsamplerate
    GIT_REPOSITORY https://github.com/libsndfile/libsamplerate.git
    GIT_TAG        master
    GIT_SHALLOW    ON
    SOURCE_DIR     ${CMAKE_SOURCE_DIR}/deps/libsamplerate
)

FetchContent_Declare(platformfolders
    GIT_REPOSITORY https://github.com/sago007/PlatformFolders.git
    GIT_TAG        master
    GIT_SHALLOW    ON
    SOURCE_DIR     ${CMAKE_SOURCE_DIR}/deps/platformfolders
)

FetchContent_Declare(concurrentqueue
    GIT_REPOSITORY https://github.com/cameron314/concurrentqueue.git
    GIT_TAG        master
    GIT_SHALLOW    ON
    SOURCE_DIR     ${CMAKE_SOURCE_DIR}/deps/concurrentqueue
)

FetchContent_Populate(concurrentqueue)

FetchContent_MakeAvailable(rtmidi akaifat filesystem rapidjson Catch2 libsamplerate platformfolders)

FetchContent_GetProperties(tl-expected)

if(NOT tl-expected_POPULATED)
  FetchContent_Populate(tl-expected)
endif()

project(mpc)

set(CMAKE_CXX_STANDARD 17)

    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)

if(APPLE)
  if (CMAKE_SYSTEM_NAME STREQUAL "iOS")
    # This is not a typo. See https://cmake.org/cmake/help/latest/variable/CMAKE_OSX_DEPLOYMENT_TARGET.html
    set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE INTERNAL "Minimum iOS deployment version")
  else()
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE INTERNAL "Minimum OS X deployment version")
  endif()
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
    ${akaifat_SOURCE_DIR}/src/main
    ${rapidjson_SOURCE_DIR}/include
    ${filesystem_SOURCE_DIR}/include
    ${tl-expected_SOURCE_DIR}/include
    ${libsamplerate_SOURCE_DIR}/include
    ${concurrentqueue_SOURCE_DIR}
)

target_link_libraries(mpc akaifat ghc_filesystem rtmidi samplerate platform_folders)

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

add_executable(mpc-tests ${_source_list_test} src/test/SamplerTest.cpp src/test/TrackTest.cpp)
target_link_libraries(mpc-tests mpc Catch2::Catch2WithMain)

if (APPLE)
    target_link_libraries(mpc-tests stdc++ "-framework Foundation -framework Security -framework DiskArbitration -framework SystemConfiguration")
endif()

_bundle_mpc_resources(mpc)

set(MPC_FACTORY_RESOURCES ${MPC_FACTORY_RESOURCES} PARENT_SCOPE)

set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES
    /.git
    /build
    /\\\\.DS_Store
)
include(CPack)
