#pragma once
#include <QMainWindow>
#include <QTextEdit>
#include <QListWidget>
#include <QSplitter>
#include <QPushButton>
#include <QLabel>
#include <QCompleter>
#include <QTimer>
#include <QCheckBox>
#include <QStringListModel>
#include "CodeEditor.h"
#include "../lexer/Lexer.h"
#include "../parser/Parser.h"
#include "../Diagnostic/DiagnosticReporter.h"
#include "../suggest_engine.h"
#include "../trie.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCheckCode();
    void onTextChanged();
    void onCursorPositionChanged();
    void onDiagnosticItemClicked(QListWidgetItem *item);
    void onClearAll();

private:
    void setupUI();
    void setupConnections();
    void updateSuggestions(const QString &prefix, int cursorPos);
    void highlightErrors();
    void populateDictionary();
    void updateDeclaredSymbols();
    void updateDictionaryFromCode();

    void onAutoCheckToggled(int);
    void performAutoCheck();
    // UI Components
    CodeEditor *codeEditor;
    QListWidget *diagnosticList;
    QLabel *statusLabel;
    QPushButton *checkButton;
    QPushButton *clearButton;
    QTimer *autoCheckTimer;
    QCheckBox *autoCheckBox;

    // Data
    DiagnosticReporter diagnostics;
    Trie dictionary;
    std::vector<std::string> keywords;
    std::vector<std::string> declaredSymbols;
    QCompleter *completer;
    QStringListModel *completerModel;
    semantics currentSemantics;
};
