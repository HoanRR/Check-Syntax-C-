
cmake_minimum_required(VERSION 3.10)
project(LexerTokenTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(diag_cli
    diag_cli.cpp
    lexer/Lexer.cpp
    parser/Parser_void.cpp
    parser/semantics.cpp
)

target_include_directories(diag_cli PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer
    ${CMAKE_CURRENT_SOURCE_DIR}/parser
    ${CMAKE_CURRENT_SOURCE_DIR}/Diagnostic
    ${CMAKE_CURRENT_SOURCE_DIR}/symboltable
)
