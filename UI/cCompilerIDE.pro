QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    CodeEditor.cpp \
    SyntaxHighlighter.cpp \
    ../lexer/Lexer.cpp \
    ../parser/Parser_void.cpp \
    ../parser/semantics.cpp \
    ../suggest_engine.cpp \
    ../trie.cpp

HEADERS += \
    MainWindow.h \
    CodeEditor.h \
    SyntaxHighlighter.h \
    ../lexer/Lexer.h \
    ../lexer/Token.h \
    ../parser/Parser.h \
    ../parser/semantics.h \
    ../Diagnostic/DiagnosticReporter.h \
    ../Diagnostic/DiagnosticsJSON.h \
    ../symboltable/symboltable.h \
    ../symboltable/type.h \
    ../suggest_engine.h \
    ../trie.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += \
    ../lexer \
    ../parser \
    ../Diagnostic \
    ../symboltable

# Enable warnings
QMAKE_CXXFLAGS += -Wall -Wextra

# Resource files (if any)
# RESOURCES += resources.qrc