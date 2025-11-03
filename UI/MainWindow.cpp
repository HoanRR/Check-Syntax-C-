#include "MainWindow.h"
#include "SyntaxHighlighter.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QMessageBox>
#include <QTextBlock>
#include "../parser/semantics.h"
#include "../preprocessor/preprocessor.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUI();
    setupConnections();
    populateDictionary();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    setWindowTitle("C Compiler IDE");
    setMinimumSize(1000, 700);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // Top buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    checkButton = new QPushButton("Kiểm tra mã (F5)", this);
    checkButton->setMinimumHeight(35);
    checkButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #4CAF50;"
        "  color: white;"
        "  border: none;"
        "  padding: 8px 16px;"
        "  font-size: 14px;"
        "  border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #3d8b40;"
        "}");

    clearButton = new QPushButton("Xóa tất cả", this);
    clearButton->setMinimumHeight(35);
    clearButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #f44336;"
        "  color: white;"
        "  border: none;"
        "  padding: 8px 16px;"
        "  font-size: 14px;"
        "  border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #da190b;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #c41e14;"
        "}");

    buttonLayout->addWidget(checkButton);
    buttonLayout->addWidget(clearButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);

    // Splitter for code editor and diagnostics
    QSplitter *splitter = new QSplitter(Qt::Vertical, this);

    // Code Editor
    codeEditor = new CodeEditor(this);
    codeEditor->setPlaceholderText("Nhập mã C của bạn tại đây...");
    QFont font("Consolas", 11);
    codeEditor->setFont(font);
    codeEditor->setStyleSheet(
        "QTextEdit {"
        "  border: 1px solid #ccc;"
        "  border-radius: 4px;"
        "  padding: 5px;"
        "  background-color: #ffffff;"
        "}");

    // Setup completer
    completerModel = new QStringListModel(this);
    completer = new QCompleter(completerModel, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    codeEditor->setCompleter(completer);

    // Add syntax highlighter
    new SyntaxHighlighter(codeEditor->document());

    splitter->addWidget(codeEditor);

    // Diagnostic Panel
    QWidget *diagnosticWidget = new QWidget(this);
    QVBoxLayout *diagLayout = new QVBoxLayout(diagnosticWidget);
    diagLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *diagLabel = new QLabel("Thông báo lỗi:", this);
    diagLabel->setStyleSheet("font-weight: bold; font-size: 13px;");
    diagLayout->addWidget(diagLabel);

    diagnosticList = new QListWidget(this);
    diagnosticList->setStyleSheet(
        "QListWidget {"
        "  border: 1px solid #ccc;"
        "  border-radius: 4px;"
        "  background-color: #fafafa;"
        "}"
        "QListWidget::item {"
        "  padding: 5px;"
        "  border-bottom: 1px solid #eee;"
        "}"
        "QListWidget::item:hover {"
        "  background-color: #e3f2fd;"
        "}"
        "QListWidget::item:selected {"
        "  background-color: #bbdefb;"
        "}");
    diagLayout->addWidget(diagnosticList);

    splitter->addWidget(diagnosticWidget);
    splitter->setStretchFactor(0, 7);
    splitter->setStretchFactor(1, 3);

    mainLayout->addWidget(splitter);

    // Status bar
    statusLabel = new QLabel("Sẵn sàng", this);
    statusLabel->setStyleSheet(
        "QLabel {"
        "  padding: 5px;"
        "  background-color: #e8f5e9;"
        "  border-radius: 3px;"
        "  font-size: 12px;"
        "}");
    mainLayout->addWidget(statusLabel);

    // Sample code
    codeEditor->setPlainText(
        "int main() {\n"
        "    int x = 10;\n"
        "    return x;\n"
        "}\n");
}

void MainWindow::setupConnections()
{
    connect(checkButton, &QPushButton::clicked, this, &MainWindow::onCheckCode);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::onClearAll);
    connect(codeEditor, &QPlainTextEdit::textChanged, this, &MainWindow::onTextChanged);
    connect(codeEditor, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::onCursorPositionChanged);
    connect(diagnosticList, &QListWidget::itemClicked, this, &MainWindow::onDiagnosticItemClicked);
}

void MainWindow::populateDictionary()
{
    keywords = {"int", "float", "double", "char", "long", "void",
                "return", "if", "else", "while", "for", "const",
                "main", "printf", "scanf"};

    for (const auto &kw : keywords)
        dictionary.insert(kw);
}

void MainWindow::onCheckCode()
{
    diagnostics.clear();
    diagnosticList->clear();
    codeEditor->clearHighlights();

    QString code = codeEditor->toPlainText();
    std::string srcCode = code.toStdString();

    // Bước 1: Xử lý #include
    Preprocessor preprocessor;
    std::vector<std::string> warnings;
    std::string processedCode = preprocessor.process(srcCode, warnings);

    // Hiển thị cảnh báo từ preprocessor
    for (const auto &warning : warnings)
    {
        QListWidgetItem *item = new QListWidgetItem("⚠️ " + QString::fromStdString(warning));
        item->setForeground(QColor(245, 124, 0));
        diagnosticList->addItem(item);
    }

    // Hiển thị các thư viện đã include thành công
    const auto &libs = preprocessor.getIncludedLibraries();
    if (!libs.empty())
    {
        QString libList = "✓ Đã nhận diện: ";
        bool first = true;
        for (const auto &lib : libs)
        {
            if (!first)
                libList += ", ";
            libList += QString::fromStdString(lib);
            first = false;
        }
        QListWidgetItem *item = new QListWidgetItem(libList);
        item->setForeground(QColor(46, 125, 50));
        diagnosticList->addItem(item);
    }

    // Bước 2: Lexer
    Lexer lexer(processedCode);
    std::vector<Token> tokens = lexer.tokenize();

    // Bước 3: Parser với Semantics
    Parser parser(tokens);
    semantics sem;
    sem.enterScope();
    vector<string> libIdentifiers = preprocessor.getLibraryIdentifiers();
    for (const auto &ident : libIdentifiers)
    {
        sem.LibraryFunction(ident);
    }

    parser.setSemantics(&sem);
    parser.setDiagnosticReporter(&diagnostics);
    parser.parseProgram();

    // Bước 4: Hiển thị kết quả
    const auto &items = diagnostics.all();
    if (items.empty() && warnings.empty())
    {
        statusLabel->setText("✓ Không có lỗi");
        statusLabel->setStyleSheet(
            "QLabel {"
            "  padding: 5px;"
            "  background-color: #e8f5e9;"
            "  border-radius: 3px;"
            "  color: #2e7d32;"
            "  font-weight: bold;"
            "}");

        QListWidgetItem *successItem = new QListWidgetItem("✓ Mã nguồn hợp lệ!");
        successItem->setForeground(QColor(46, 125, 50));
        diagnosticList->addItem(successItem);
    }
    else
    {
        int errorCount = 0;
        for (const auto &diag : items)
            if (diag.severity == DiagSeverity::Error)
                errorCount++;

        if (errorCount > 0)
        {
            statusLabel->setText(QString("✗ Tìm thấy %1 lỗi").arg(errorCount));
            statusLabel->setStyleSheet(
                "QLabel {"
                "  padding: 5px;"
                "  background-color: #ffebee;"
                "  border-radius: 3px;"
                "  color: #c62828;"
                "  font-weight: bold;"
                "}");
        }
        else
        {
            statusLabel->setText(QString("⚠ Có %1 cảnh báo").arg(warnings.size()));
            statusLabel->setStyleSheet(
                "QLabel {"
                "  padding: 5px;"
                "  background-color: #fff3e0;"
                "  border-radius: 3px;"
                "  color: #f57c00;"
                "  font-weight: bold;"
                "}");
        }

        for (const auto &diag : items)
        {
            QString severity = (diag.severity == DiagSeverity::Error) ? "🔴" : "⚠️";
            QString msg = QString("%1 [Dòng %2, Cột %3] %4")
                              .arg(severity)
                              .arg(diag.line)
                              .arg(diag.col)
                              .arg(QString::fromStdString(diag.message));

            QListWidgetItem *item = new QListWidgetItem(msg);
            item->setData(Qt::UserRole, diag.line);
            item->setData(Qt::UserRole + 1, diag.col);
            item->setData(Qt::UserRole + 2, diag.length);

            if (diag.severity == DiagSeverity::Error)
                item->setForeground(QColor(198, 40, 40));
            else
                item->setForeground(QColor(245, 124, 0));

            diagnosticList->addItem(item);
        }

        highlightErrors();
    }
}

void MainWindow::highlightErrors()
{
    const auto &items = diagnostics.all();
    for (const auto &diag : items)
    {
        QColor color = (diag.severity == DiagSeverity::Error)
                           ? QColor(255, 205, 210)
                           : QColor(255, 245, 157);
        codeEditor->highlightLine(diag.line, diag.col, diag.length, color);
    }
}

void MainWindow::onTextChanged()
{
    // Real-time syntax highlighting (simple version)
    // For production, use QSyntaxHighlighter
}

void MainWindow::onCursorPositionChanged()
{
    QTextCursor cursor = codeEditor->textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    QString word = cursor.selectedText();

    if (word.length() >= 2)
    {
        updateSuggestions(word, cursor.position());
    }
}

void MainWindow::updateSuggestions(const QString &prefix, int cursorPos)
{
    std::string prefixStr = prefix.toStdString();
    std::vector<std::string> allWords = dictionary.getAllWords();

    auto suggestions = smartSuggestList(prefixStr, allWords, 10);

    QStringList suggestionList;
    for (const auto &s : suggestions)
        suggestionList << QString::fromStdString(s);

    if (!suggestionList.isEmpty())
    {
        completerModel->setStringList(suggestionList);
        completer->setCompletionPrefix(prefix);

        if (completer->completionCount() > 0)
        {
            QRect cr = codeEditor->cursorRect();
            cr.setWidth(completer->popup()->sizeHintForColumn(0) + completer->popup()->verticalScrollBar()->sizeHint().width());
            completer->complete(cr);
        }
    }
}

void MainWindow::onDiagnosticItemClicked(QListWidgetItem *item)
{
    int line = item->data(Qt::UserRole).toInt();
    int col = item->data(Qt::UserRole + 1).toInt();

    QTextCursor cursor = codeEditor->textCursor();
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, line - 1);
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, col - 1);

    codeEditor->setTextCursor(cursor);
    codeEditor->setFocus();
}

void MainWindow::onClearAll()
{
    codeEditor->clear();
    diagnosticList->clear();
    diagnostics.clear();
    codeEditor->clearHighlights();
    statusLabel->setText("Sẵn sàng");
    statusLabel->setStyleSheet(
        "QLabel {"
        "  padding: 5px;"
        "  background-color: #e8f5e9;"
        "  border-radius: 3px;"
        "  font-size: 12px;"
        "}");
}
