add_executable(
  file_reader_test1
  file_reader_test1.cpp
)

target_link_libraries(
  file_reader_test1
  gcl-lexical
)

add_executable(string_test string_test.cpp)

# add_executable(grammar_test tests.cpp)

add_executable(syntatic_table table_tests.cpp)

add_executable(tree_test tree_test.cpp)
