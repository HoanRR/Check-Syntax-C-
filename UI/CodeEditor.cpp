#include "CodeEditor.h"
#include "SuggestionWidget.h"
#include <QAbstractItemView>
#include <QScrollBar>

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    // Tạo suggestion widget
    suggestionWidget = new SuggestionWidget(this);
    suggestionWidget->hide();

    connect(suggestionWidget, &SuggestionWidget::suggestionSelected,
            this, &CodeEditor::insertSuggestion);

    connect(this, &QPlainTextEdit::blockCountChanged,
            this, &CodeEditor::updateLineNumberAreaWidth);

    connect(this, &QPlainTextEdit::updateRequest,
            this, &CodeEditor::updateLineNumberArea);

    updateLineNumberAreaWidth(0);
}

CodeEditor::~CodeEditor() {}

void CodeEditor::showSuggestions(const QStringList &suggestions, const QString &prefix)
{
    if (suggestions.isEmpty())
    {
        suggestionWidget->hide();
        return;
    }

    currentPrefix = prefix;
    suggestionWidget->clear();
    suggestionWidget->addItems(suggestions);

    // Đặt vị trí popup ngay dưới con trỏ
    QRect cr = cursorRect();
    QPoint globalPos = mapToGlobal(cr.bottomLeft());

    suggestionWidget->move(globalPos);
    suggestionWidget->setFixedWidth(250);

    // Tính toán chiều cao dựa trên số lượng items
    int itemHeight = suggestionWidget->sizeHintForRow(0);
    int height = qMin(200, itemHeight * suggestions.count() + 10);
    suggestionWidget->setFixedHeight(height);

    suggestionWidget->setCurrentRow(0);
    suggestionWidget->show();
    suggestionWidget->setFocus(Qt::PopupFocusReason);
}

void CodeEditor::hideSuggestions()
{
    suggestionWidget->hide();
    currentPrefix.clear();
}

void CodeEditor::insertSuggestion(const QString &text)
{
    QTextCursor tc = textCursor();

    // Xóa prefix đã gõ
    tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, currentPrefix.length());
    tc.removeSelectedText();

    // Chèn suggestion
    tc.insertText(text);
    setTextCursor(tc);

    hideSuggestions();
    setFocus();
}

QString CodeEditor::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void CodeEditor::focusInEvent(QFocusEvent *e)
{
    QPlainTextEdit::focusInEvent(e);
}

void CodeEditor::focusOutEvent(QFocusEvent *e)
{
    // Chỉ ẩn suggestion nếu focus không chuyển sang suggestion widget
    if (e->reason() != Qt::PopupFocusReason)
    {
        hideSuggestions();
    }
    QPlainTextEdit::focusOutEvent(e);
}

void CodeEditor::keyPressEvent(QKeyEvent *e)
{
    // Nếu suggestion widget đang hiển thị
    if (suggestionWidget->isVisible())
    {
        // Các phím điều hướng và chọn
        if (e->key() == Qt::Key_Up || e->key() == Qt::Key_Down ||
            e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
        {
            suggestionWidget->event(e);
            return;
        }
        else if (e->key() == Qt::Key_Escape)
        {
            hideSuggestions();
            return;
        }
        // Các phím khác sẽ ẩn suggestion và xử lý bình thường
        else if (e->key() == Qt::Key_Space ||
                 e->key() == Qt::Key_Semicolon ||
                 e->key() == Qt::Key_ParenLeft ||
                 e->key() == Qt::Key_ParenRight)
        {
            hideSuggestions();
        }
    }

    // Auto-indent với Tab
    if (e->key() == Qt::Key_Tab)
    {
        insertPlainText("    "); // 4 spaces
        return;
    }

    // Auto-indent với Enter
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
    {
        QTextCursor cursor = textCursor();
        QString currentLine = cursor.block().text();

        // Đếm số khoảng trắng đầu dòng
        int spaces = 0;
        for (QChar c : currentLine)
        {
            if (c == ' ')
                spaces++;
            else if (c == '\t')
                spaces += 2;
            else
                break;
        }

        // Thêm indent nếu dòng kết thúc bằng {
        if (currentLine.trimmed().endsWith('{'))
            spaces += 4;

        QPlainTextEdit::keyPressEvent(e);

        // Chèn khoảng trắng cho indent
        cursor = textCursor();
        cursor.insertText(QString(spaces, ' '));
        return;
    }

    // Auto-close brackets
    if (e->key() == Qt::Key_BraceLeft)
    {
        QPlainTextEdit::keyPressEvent(e);
        textCursor().insertText("}");
        moveCursor(QTextCursor::Left);
        return;
    }
    if (e->key() == Qt::Key_ParenLeft)
    {
        QPlainTextEdit::keyPressEvent(e);
        textCursor().insertText(")");
        moveCursor(QTextCursor::Left);
        return;
    }

    // Xử lý phím bình thường
    QPlainTextEdit::keyPressEvent(e);

    // Sau khi xử lý phím, kiểm tra xem có cần hiện suggestion không
    // Trigger suggestion sẽ được xử lý bởi MainWindow qua signal textChanged
}

void CodeEditor::highlightLine(int line, int col, int length, const QColor &color)
{
    highlights.push_back({line, col, length, color});

    QTextCursor cursor(document()->findBlockByLineNumber(line - 1));
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, col - 1);
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, length);

    QTextCharFormat fmt;
    fmt.setBackground(color);
    cursor.setCharFormat(fmt);
}

void CodeEditor::clearHighlights()
{
    highlights.clear();

    QTextCursor cursor(document());
    cursor.select(QTextCursor::Document);
    QTextCharFormat fmt;
    fmt.setBackground(Qt::white);
    cursor.setCharFormat(fmt);
}

// Line number area
int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, document()->blockCount());
    while (max >= 10)
    {
        max /= 10;
        ++digits;
    }

    int space = 10 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(),
                                      lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QColor(240, 240, 240));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(QColor(120, 120, 120));
            painter.drawText(0, top, lineNumberArea->width() - 5,
                             fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}