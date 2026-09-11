cmake_minimum_required(VERSION 3.10.0)
project(palm-tree VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

include_directories(include/)

add_executable(palm_tree src/palm_tree.c
    src/phrase.c)


enable_testing()
add_executable(phrase_segment_test
    test/phrase_segment_test.c
    src/phrase.c
)
add_test(NAME phrase_segment_test
         COMMAND phrase_segment_test)
