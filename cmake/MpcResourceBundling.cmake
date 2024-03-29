include(cmake/CMakeRC.cmake)

set(_mpc_resources_root ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set(_mpc_test_resources_root ${CMAKE_CURRENT_SOURCE_DIR}/test-resources)

function(_bundle_resources)

  file(GLOB_RECURSE MPC_RESOURCES "${_mpc_resources_root}/*")
  list(FILTER MPC_RESOURCES EXCLUDE REGEX "\\.DS_Store$")

  if (APPLE)
    foreach(RESOURCE ${MPC_RESOURCES})
      get_filename_component(SOURCE_DIR "${RESOURCE}" DIRECTORY)
      string(REPLACE "${_mpc_resources_root}" "" RELATIVE_DIR "${SOURCE_DIR}")
      set_source_files_properties(${RESOURCE} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources${RELATIVE_DIR}")
    endforeach()
    target_sources(mpc-tests PRIVATE ${MPC_RESOURCES})
  else()
    cmrc_add_resource_library(
            mpc_resources
            ALIAS mpc::rc
            NAMESPACE mpc
            WHENCE ${_mpc_resources_root}
            ${MPC_RESOURCES}
    )

    target_link_libraries(mpc mpc::rc)
  endif()

  file(GLOB_RECURSE MPC_TEST_RESOURCES "${_mpc_test_resources_root}/*")
  list(FILTER MPC_TEST_RESOURCES EXCLUDE REGEX "\\.DS_Store$")

  cmrc_add_resource_library(
          mpc_test_resources
          ALIAS mpctest::rc
          NAMESPACE mpctest
          WHENCE ${_mpc_test_resources_root}
          PREFIX test
          ${MPC_TEST_RESOURCES}
  )

  target_link_libraries(mpc-tests mpctest::rc)
endfunction()
