#pragma once
#include <QTextEdit>
#include <QCompleter>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QTextBlock>
#include <QPainter>
#include <vector>
#include <QPaintEvent>
#include <QTextBlock>
#include <QPainter>
#include <QPlainTextEdit>

class LineNumberArea;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = nullptr);
    ~CodeEditor();

    void setCompleter(QCompleter *c);
    QCompleter *completer() const;
    
    void highlightLine(int line, int col, int length, const QColor &color);
    void clearHighlights();

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void insertCompletion(const QString &completion);
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QString textUnderCursor() const;
    
    QCompleter *m_completer;
    QWidget *lineNumberArea;
    
    struct Highlight {
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

    QSize sizeHint() const override {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;
};
