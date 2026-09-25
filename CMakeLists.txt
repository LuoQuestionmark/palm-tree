cmake_minimum_required(VERSION 3.10.0)
project(palm-tree VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_COMPILER clang)

add_compile_options(-Wall -Wextra)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

# add_compile_options(-pg)

include_directories(include/)
include_directories(extern/)

add_library(palm_patterns
    src/patterns/digits.c
    src/patterns/latin.c
    src/utils/utf8_utils.c
)

add_library(palm_phrase
    src/phrase.c
    src/phrase_filter.c
    src/utils/bloom_filter.c
    src/utils/dict.c
    src/utils/utf8_utils.c
    src/words.c
    extern/lookup3.c
)
target_link_libraries(palm_phrase
    palm_patterns
)

add_library(palm_postag
    src/postag/postag.c
    src/patterns/cn_aux.c
    src/utils/dict.c
    src/words.c
)
target_link_libraries(palm_postag
    palm_phrase
)

add_executable(palm_tree
    src/palm_tree.c
    src/words.c
)
target_link_libraries(palm_tree PRIVATE
    palm_phrase
    palm_patterns
    palm_postag
)

enable_testing()

add_executable(segment_test
    test/segment_test.c
)
target_link_libraries(segment_test PRIVATE
    palm_phrase
    palm_patterns
)
add_test(
    NAME segment_test
    COMMAND segment_test
)

add_executable(phrase_test
    test/phrase_test.c
)
target_link_libraries(phrase_test PRIVATE
    palm_phrase
    palm_patterns
)
add_test(
    NAME phrase_test
    COMMAND phrase_test
)

add_executable(phrase_filter_test
    test/phrase_filter_test.c
)
target_link_libraries(phrase_filter_test PRIVATE
    palm_phrase
    palm_patterns
)
add_test(
    NAME phrase_filter_test
    COMMAND phrase_filter_test
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_executable(words_test
    test/words_test.c
    src/phrase.c
    src/words.c
    src/phrase_filter.c
    src/utils/bloom_filter.c
    src/utils/dict.c
    src/utils/utf8_utils.c
    src/patterns/digits.c
    extern/lookup3.c
)
target_link_libraries(words_test PRIVATE
    palm_patterns
)
add_test(
    NAME words_test
    COMMAND words_test
)

add_executable(dict_test
    test/dict_test.c
    src/words.c
    src/utils/bloom_filter.c
    src/utils/dict.c
    src/utils/utf8_utils.c
    extern/lookup3.c
)
target_link_libraries(dict_test PRIVATE
    palm_phrase
)
add_test(
    NAME dict_test
    COMMAND dict_test
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_executable(digits_test
    test/digits_test.c
    src/patterns/digits.c
    src/utils/utf8_utils.c
)
add_test(
    NAME digits_test
    COMMAND digits_test
)

add_executable(latin_test
    test/latin_test.c
    src/patterns/latin.c
    src/utils/utf8_utils.c
)
add_test(
    NAME latin_test
    COMMAND latin_test
)

add_executable(bloom_test
    test/bloom_test.c
    src/utils/bloom_filter.c
    src/utils/utf8_utils.c
    extern/lookup3.c
)
add_test(
    NAME bloom_test
    COMMAND bloom_test
)
