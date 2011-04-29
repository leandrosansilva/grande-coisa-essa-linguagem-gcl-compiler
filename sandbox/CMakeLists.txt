add_executable(automata_basic rec.c)

add_executable(parse_integer inteiros.c)

add_executable(table automata_table.cpp)
target_link_libraries(table gcl-lexical)

add_executable(comment comentario.cpp)
target_link_libraries(comment gcl-lexical)

add_executable(stack_automata stack_automata.cpp)
