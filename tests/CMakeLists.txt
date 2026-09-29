add_executable(unit_tests
  test_main.cpp
  frame_schedule_test.cpp
  frame_stats_test.cpp
  input_test.cpp
)
target_link_libraries(unit_tests PRIVATE app_core glfw)
target_compile_definitions(unit_tests PRIVATE GLFW_INCLUDE_NONE)

add_test(NAME unit_tests COMMAND unit_tests)

add_executable(gl_tests
  gl_test_main.cpp
  window_test.cpp
  renderer_test.cpp
)
target_link_libraries(gl_tests PRIVATE app_core glfw glad)
target_compile_definitions(gl_tests PRIVATE GLFW_INCLUDE_NONE)

add_test(NAME gl_tests COMMAND gl_tests)
set_tests_properties(gl_tests PROPERTIES SKIP_RETURN_CODE 77 LABELS gl)
