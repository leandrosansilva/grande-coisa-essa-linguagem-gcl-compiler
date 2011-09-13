add_executable(automata_basic rec.c)

add_executable(parse_integer inteiros.c)

add_executable(gcl_example gcl_example.cpp)
target_link_libraries(gcl_example gcl-lexical)

add_executable(comment comentario.cpp)
target_link_libraries(comment gcl-lexical)

add_executable(gramar_examples gramar_examples.cpp)
