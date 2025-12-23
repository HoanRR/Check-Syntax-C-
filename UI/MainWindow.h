#pragma once
#include "CodeEditor.h"
#include "../lexer/Lexer.h"
#include "../parser/Parser.h"
#include "../Diagnostic/DiagnosticReporter.h"
#include "../Trie/trie.h"

#include <QMainWindow>
#include <QTextEdit>
#include <QListWidget>
#include <QSplitter>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QCheckBox>

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
    void onAutoCheckToggled(int);

private:
    void setupUI();
    void setupConnections();
    void updateSuggestions();
    void highlightErrors();
    void populateDictionary();
    void updateDictionaryFromCode();
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
    semantics currentSemantics;
};