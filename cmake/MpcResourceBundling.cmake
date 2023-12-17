include(cmake/CMakeRC.cmake)

set(_mpc_resources_root ${CMAKE_CURRENT_SOURCE_DIR}/resources)

function(_bundle_resources)

  set(total_list "")

  _add_resources(${_mpc_resources_root} screens json "${total_list}")
  _add_resources(${_mpc_resources_root} screens/bg png "${total_list}")
  _add_resources(${_mpc_resources_root} fonts "fnt;bmp" "${total_list}")
  _add_resources(${_mpc_resources_root} audio wav "${total_list}")
  _add_resources(${_mpc_resources_root} midicontrolpresets vmp "${total_list}")
  _add_resources(${_mpc_resources_root} demodata/TEST1 "SND;PGM;APS;MID;ALL" "${total_list}")
  _add_resources(${_mpc_resources_root} demodata/TEST2 "SND;PGM;APS;MID;ALL" "${total_list}")

  if (NOT (APPLE AND NOT IOS))
    cmrc_add_resource_library(
            mpc_resources
            ALIAS mpc::rc
            NAMESPACE mpc
            WHENCE ${_mpc_resources_root}
            ${total_list}
    )

    target_link_libraries(mpc mpc::rc)
  endif()

  set(total_list "")

  _add_resources(${_mpc_resources_root} test/ProgramLoading/program1 "PGM;SND" "${total_list}")
  _add_resources(${_mpc_resources_root} test/ProgramLoading/program2 "PGM;SND" "${total_list}")
  _add_resources(${_mpc_resources_root} test/ApsLoading "APS;SND" "${total_list}")
  _add_resources(${_mpc_resources_root} test/Sampler SND "${total_list}")
  _add_resources(${_mpc_resources_root} test/ExternalClock txt "${total_list}")

  cmrc_add_resource_library(
    mpc_test_resources
    ALIAS mpctest::rc
    NAMESPACE mpctest
    WHENCE ${_mpc_resources_root}
    ${total_list}
    )

  target_link_libraries(mpc-tests mpctest::rc)

  if (APPLE AND NOT IOS)

    file(GLOB_RECURSE MPC_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/resources/*")

    list(FILTER MPC_RESOURCES EXCLUDE REGEX "${CMAKE_CURRENT_SOURCE_DIR}/resources/test/.*")

    foreach(RESOURCE ${MPC_RESOURCES})
      get_filename_component(SOURCE_DIR "${RESOURCE}" DIRECTORY)
      string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/resources" "" RELATIVE_DIR "${SOURCE_DIR}")
      set_source_files_properties(${RESOURCE} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources${RELATIVE_DIR}")
    endforeach()

    target_sources(mpc PUBLIC ${MPC_RESOURCES})

    string(REPLACE "resources" "Resources" total_list_upper_case ${total_list})

    set_target_properties(mpc PROPERTIES
            RESOURCE ${total_list_upper_case}
    )
  endif()

endfunction()

function(_add_resources _rsrc_root_path _sub_dir _extensions _total_list)
  foreach(_extension IN LISTS _extensions)
    file(
      GLOB _list
      LIST_DIRECTORIES false
      RELATIVE "${CMAKE_CURRENT_LIST_DIR}"
      "${_rsrc_root_path}/${_sub_dir}/*.${_extension}"
      )
    list (APPEND _total_list ${_list})
  endforeach ()
  set (total_list ${_total_list} PARENT_SCOPE)
endfunction()
