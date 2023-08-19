include(cmake/CMakeRC.cmake)

set(_mpc_resources_root ${CMAKE_CURRENT_SOURCE_DIR}/resources)

function(_bundle_mpc_resources _target_name)
  set(total_list "")

  _add_mpc_resource_files(${_mpc_resources_root} screens json "${total_list}")
  _add_mpc_resource_files(${_mpc_resources_root} screens/bg png "${total_list}")
  _add_mpc_resource_files(${_mpc_resources_root} fonts "fnt;bmp" "${total_list}")
  _add_mpc_resource_files(${_mpc_resources_root} audio wav "${total_list}")
  _add_mpc_resource_files(${_mpc_resources_root} midicontrolpresets vmp "${total_list}")
  _add_mpc_resource_files(${_mpc_resources_root} demodata/TEST1 "SND;PGM;APS;MID;ALL" "${total_list}")
  _add_mpc_resource_files(${_mpc_resources_root} demodata/TEST2 "SND;PGM;APS;MID;ALL" "${total_list}")

  if (NOT (APPLE AND NOT IOS))
    cmrc_add_resource_library(
            mpc_resources
            ALIAS mpc::rc
            NAMESPACE mpc
            WHENCE ${_mpc_resources_root}
            ${total_list}
    )

    target_link_libraries(${_target_name} mpc::rc)
  endif()

  set(total_list_test "")

  _add_mpc_test_resource_files(${_mpc_resources_root} test/ProgramLoading/program1 "PGM;SND" "${total_list_test}")
  _add_mpc_test_resource_files(${_mpc_resources_root} test/ProgramLoading/program2 "PGM;SND" "${total_list_test}")
  _add_mpc_test_resource_files(${_mpc_resources_root} test/ApsLoading "APS;SND" "${total_list_test}")
  _add_mpc_test_resource_files(${_mpc_resources_root} test/Sampler SND "${total_list_test}")

  cmrc_add_resource_library(
    mpc_test_resources
    ALIAS mpctest::rc
    NAMESPACE mpctest
    WHENCE ${_mpc_resources_root}
    ${total_list_test}
    )

  target_link_libraries(${_target_name}-tests mpctest::rc)
endfunction()

function(_add_mpc_resource_files _rsrc_root_path _sub_dir _extensions _total_list)
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

function(_add_mpc_test_resource_files _rsrc_root_path _sub_dir _extensions _total_list)
  foreach(_extension IN LISTS _extensions)
    file(
      GLOB _list
      LIST_DIRECTORIES false
      RELATIVE "${CMAKE_CURRENT_LIST_DIR}"
      "${_rsrc_root_path}/${_sub_dir}/*.${_extension}"
      )
    list (APPEND _total_list ${_list})
  endforeach ()
  set (total_list_test ${_total_list} PARENT_SCOPE)
endfunction()
