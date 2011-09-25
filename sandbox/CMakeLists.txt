add_executable(automata_basic rec.c)

add_executable(parse_integer inteiros.c)

add_executable(gcl_example gcl_example.cpp)
target_link_libraries(gcl_example gcl-lexical)

add_executable(comment comentario.cpp)
target_link_libraries(comment gcl-lexical)

add_executable(gramar_examples gramar_examples.cpp)

add_executable(lisp lisp_grammar.cpp)
target_link_libraries(lisp gcl-lexical)

add_executable(list_grammar list_grammar.cpp)
target_link_libraries(list_grammar gcl-lexical)

add_executable(ambiguous_example ambiguous_example.cpp)
target_link_libraries(ambiguous_example gcl-lexical)
