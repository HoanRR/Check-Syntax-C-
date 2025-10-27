cmake_minimum_required(VERSION 3.10)
project(CSyntaxChecker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Đường dẫn SFML
set(SFML_ROOT "C:/SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit/SFML-2.5.1")
set(SFML_DIR "${SFML_ROOT}/lib/cmake/SFML")

# Include và library directories
include_directories(${SFML_ROOT}/include)
link_directories(${SFML_ROOT}/lib)

target_include_directories(diag_cli PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer
    ${CMAKE_CURRENT_SOURCE_DIR}/parser
    ${CMAKE_CURRENT_SOURCE_DIR}/Diagnostic
    ${CMAKE_CURRENT_SOURCE_DIR}/symboltable
)

# Executable Editor với SFML
add_executable(editor
    main.cpp
    UI/UI.cpp
    lexer/Lexer.cpp
    parser/Parser_void.cpp
    parser/semantics.cpp
)

target_include_directories(editor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer
    ${CMAKE_CURRENT_SOURCE_DIR}/parser
    ${CMAKE_CURRENT_SOURCE_DIR}/Diagnostic
    ${CMAKE_CURRENT_SOURCE_DIR}/symboltable
    ${CMAKE_CURRENT_SOURCE_DIR}/Editor
)

target_link_libraries(editor PRIVATE 
    sfml-graphics
    sfml-window
    sfml-system
)

# Copy SFML DLLs vào thư mục build
if(WIN32)
    file(GLOB SFML_DLLS "${SFML_ROOT}/bin/*.dll")
    foreach(dll ${SFML_DLLS})
        add_custom_command(TARGET editor POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${dll} $<TARGET_FILE_DIR:editor>
        )
    endforeach()
endif()