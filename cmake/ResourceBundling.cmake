include(cmake/CMakeRC.cmake)

set(_mpc_resources_root ${CMAKE_CURRENT_SOURCE_DIR}/resources)

function(_bundle_mpc_resources _target_name)
  set(total_list "")
  set(total_list_test "")

  _add_resource_files(${_target_name} ${_mpc_resources_root} screens json "${total_list}")
  _add_resource_files(${_target_name} ${_mpc_resources_root} screens/bg bmp "${total_list}")
  _add_resource_files(${_target_name} ${_mpc_resources_root} fonts fnt "${total_list}")
  _add_resource_files(${_target_name} ${_mpc_resources_root} fonts bmp "${total_list}")
  _add_resource_files(${_target_name} ${_mpc_resources_root} audio wav "${total_list}")
  _add_resource_files(${_target_name} ${_mpc_resources_root} midicontrolpresets vmp "${total_list}")

  _add_test_resource_files(${_target_name}-tests ${_mpc_resources_root} test/ProgramLoading/program1 PGM "${total_list_test}")
  _add_test_resource_files(${_target_name}-tests ${_mpc_resources_root} test/ProgramLoading/program1 SND "${total_list_test}")
  _add_test_resource_files(${_target_name}-tests ${_mpc_resources_root} test/ProgramLoading/program2 PGM "${total_list_test}")
  _add_test_resource_files(${_target_name}-tests ${_mpc_resources_root} test/ProgramLoading/program2 SND "${total_list_test}")
  _add_test_resource_files(${_target_name}-tests ${_mpc_resources_root} test/ApsLoading APS "${total_list_test}")
  _add_test_resource_files(${_target_name}-tests ${_mpc_resources_root} test/ApsLoading SND "${total_list_test}")
  _add_test_resource_files(${_target_name}-tests ${_mpc_resources_root} test/Sampler SND "${total_list_test}")

  cmrc_add_resource_library(
    mpc_resources
    ALIAS mpc::rc
    NAMESPACE mpc
    WHENCE ${_mpc_resources_root}
    ${total_list}
    )

  cmrc_add_resource_library(
    mpc_test_resources
    ALIAS mpctest::rc
    NAMESPACE mpctest
    WHENCE ${_mpc_resources_root}
    ${total_list_test}
    )
  target_link_libraries(${_target_name} mpc::rc)
  target_link_libraries(${_target_name}-tests mpctest::rc)
endfunction()

function(_add_resource_files _target_name _rsrc_root_path _sub_dir _extension _total_list)
  file(
    GLOB _list
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_LIST_DIR}"
    "${_rsrc_root_path}/${_sub_dir}/*.${_extension}"
    )

  list (APPEND _total_list ${_list})
  set (total_list ${_total_list} PARENT_SCOPE)
endfunction()

function(_add_test_resource_files _target_name _rsrc_root_path _sub_dir _extension _total_list)
  file(
    GLOB _list
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_LIST_DIR}"
    "${_rsrc_root_path}/${_sub_dir}/*.${_extension}"
    )

  list (APPEND _total_list ${_list})
  set (total_list_test ${_total_list} PARENT_SCOPE)
endfunction()
