aux_source_directory(
  ${CMAKE_CURRENT_SOURCE_DIR} SRC
)

add_executable(
  src_test
  ${SRC}
)

target_link_libraries(
  src_test
  GTest::gtest_main
  shlog
)

gtest_discover_tests(src_test)