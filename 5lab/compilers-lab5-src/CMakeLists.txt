cmake_minimum_required(VERSION 3.10)
project(CoolParser)

find_package(BISON)
find_package(FLEX)

bison_target(Parser src/cool.y
            ${CMAKE_CURRENT_BINARY_DIR}/cool-bison-parser.cc
            DEFINES_FILE
            ${CMAKE_CURRENT_BINARY_DIR}/cool-bison-parser.h
            COMPILE_FLAGS
            "-v -y -b cool --debug -p cool_yy")

flex_target(Lexer src/cool.flex
            ${CMAKE_CURRENT_BINARY_DIR}/cool-flex-lexer.cc
            COMPILE_FLAGS
            "-d")

add_flex_bison_dependency(Lexer Parser)

set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED True)

add_executable(parser
            src/parser-phase.cc
            src/utilities.cc
            src/stringtab.cc
            src/cool-tree.cc
            ${BISON_Parser_OUTPUTS}
            ${FLEX_Lexer_OUTPUTS} )

target_include_directories(parser PRIVATE ${CMAKE_CURRENT_BINARY_DIR} src)

target_compile_options(parser PRIVATE -Wall -Wno-unused -Wno-deprecated -Wno-write-strings -Wno-free-nonheap-object )