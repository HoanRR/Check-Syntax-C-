#pragma once
#include <QPlainTextEdit>
#include <QKeyEvent>
#include <QTextBlock>
#include <QPainter>
#include <vector>
#include <QPaintEvent>

class LineNumberArea;
class SuggestionWidget;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = nullptr);
    ~CodeEditor();

    void showSuggestions(const QStringList &suggestions, const QString &prefix);
    void hideSuggestions();

    void highlightLine(int line, int col, int length, const QColor &color);
    void clearHighlights();

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;
    void resizeEvent(QResizeEvent *event) override;
    void focusOutEvent(QFocusEvent *e) override;

private slots:
    void insertSuggestion(const QString &text);
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QString textUnderCursor() const;

    SuggestionWidget *suggestionWidget;
    QWidget *lineNumberArea;
    QString currentPrefix;

    struct Highlight
    {
        int line;
        int col;
        int length;
        QColor color;
    };
    std::vector<Highlight> highlights;
};

// Line number area widget
class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor) {}

    QSize sizeHint() const override
    {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;
};