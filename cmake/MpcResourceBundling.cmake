set(_mpc_resources_root ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set(_mpc_test_resources_root ${CMAKE_CURRENT_SOURCE_DIR}/test-resources)

function(_generate_mpc_required_resource_manifest)
  set(_required_dirs
      "${_mpc_resources_root}/audio"
      "${_mpc_resources_root}/fonts"
      "${_mpc_resources_root}/screens")

  set(_required_resources)
  foreach(_dir IN LISTS _required_dirs)
    file(GLOB_RECURSE _dir_resources "${_dir}/*")
    list(APPEND _required_resources ${_dir_resources})
  endforeach()

  list(FILTER _required_resources EXCLUDE REGEX "\\.DS_Store$")
  list(SORT _required_resources)

  set(_generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
  file(MAKE_DIRECTORY "${_generated_dir}")

  set(_header_path "${_generated_dir}/MpcRequiredResources.hpp")
  set(_header_content "#pragma once\n\n#include <array>\n#include <string_view>\n\nnamespace mpc::required_resources\n{\n")
  list(LENGTH _required_resources _required_count)
  string(APPEND _header_content
         "inline constexpr std::array<std::string_view, ${_required_count}> paths{\n")

  foreach(_resource IN LISTS _required_resources)
    file(RELATIVE_PATH _relative_path "${_mpc_resources_root}" "${_resource}")
    string(APPEND _header_content "    \"${_relative_path}\",\n")
  endforeach()

  string(APPEND _header_content "};\n}\n")
  file(WRITE "${_header_path}" "${_header_content}")
endfunction()

function(_bundle_resources)

  _generate_mpc_required_resource_manifest()

  file(GLOB_RECURSE MPC_RESOURCES CONFIGURE_DEPENDS "${_mpc_resources_root}/*")
  list(FILTER MPC_RESOURCES EXCLUDE REGEX "\\.DS_Store$")

  if (APPLE)
    foreach(RESOURCE ${MPC_RESOURCES})
      get_filename_component(SOURCE_DIR "${RESOURCE}" DIRECTORY)
      string(REPLACE "${_mpc_resources_root}" "" RELATIVE_DIR "${SOURCE_DIR}")
      set_source_files_properties(${RESOURCE} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources${RELATIVE_DIR}")
    endforeach()
    target_sources(mpc-tests PRIVATE ${MPC_RESOURCES})
  else()
    add_library(mpc_resources STATIC)
    add_library(mpc::rc ALIAS mpc_resources)
    target_compile_features(mpc_resources PUBLIC cxx_std_20)
    res_embed_add(mpc_resources
      FILES ${MPC_RESOURCES}
      BASE_DIRECTORY "${_mpc_resources_root}"
      NAMESPACE MpcResources
      CATEGORY mpc
      TU_COUNT 1)

    target_link_libraries(mpc mpc::rc)
  endif()

  file(GLOB_RECURSE MPC_TEST_RESOURCES CONFIGURE_DEPENDS "${_mpc_test_resources_root}/*")
  list(FILTER MPC_TEST_RESOURCES EXCLUDE REGEX "\\.DS_Store$")

  if(APPLE)
    foreach(resource IN LISTS MPC_TEST_RESOURCES)
      file(RELATIVE_PATH relative_path "${_mpc_test_resources_root}" "${resource}")
      get_filename_component(relative_dir "${relative_path}" DIRECTORY)
      set_source_files_properties("${resource}" PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources/test/${relative_dir}")
    endforeach()
    target_sources(mpc-tests PRIVATE ${MPC_TEST_RESOURCES})
  else()
    res_embed_add(mpc-tests
      FILES ${MPC_TEST_RESOURCES}
      BASE_DIRECTORY "${_mpc_test_resources_root}"
      NAMESPACE MpcTestResources
      CATEGORY mpctest
      TU_COUNT 1)
  endif()
endfunction()
