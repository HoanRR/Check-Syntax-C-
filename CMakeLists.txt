cmake_minimum_required(VERSION 3.16)
project(CCompilerIDE LANGUAGES CXX)

# C++ & Qt build helpers
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Tìm Qt5 hoặc Qt6 (Widgets)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Nguồn
set(SRC
    preprocessor/preprocessor.cpp
    UI/main.cpp
    UI/MainWindow.cpp
    UI/CodeEditor.cpp
    UI/SyntaxHighlighter.cpp
    lexer/Lexer.cpp
    parser/Parser_void.cpp
    parser/semantics.cpp
    trie.cpp
)

# Header (liệt kê để IDE hiện file, không bắt buộc cho build)
set(HDR
    preprocessor/preprocessor.h
    UI/MainWindow.h
    UI/CodeEditor.h
    UI/SuggestionWidget.h
    UI/SyntaxHighlighter.h
    lexer/Lexer.h
    lexer/Token.h
    parser/Parser.h
    parser/semantics.h
    Diagnostic/DiagnosticReporter.h
    Diagnostic/DiagnosticsJSON.h
    symboltable/symboltable.h
    symboltable/type.h
    trie.h
)

# Nếu bạn không muốn hiện console trên Windows, thêm WIN32 sau tên target
add_executable(${PROJECT_NAME} ${SRC} ${HDR})

target_include_directories(${PROJECT_NAME} PRIVATE
    UI
    lexer
    parser
    Diagnostic
    symboltable
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
)
