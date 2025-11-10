#include "CodeEditor.h"
#include <QAbstractItemView>
#include <QScrollBar>

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent), m_completer(nullptr)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, &QPlainTextEdit::blockCountChanged,
            this, &CodeEditor::updateLineNumberAreaWidth);

    connect(this, &QPlainTextEdit::updateRequest,
            this, &CodeEditor::updateLineNumberArea);

    updateLineNumberAreaWidth(0);

}

CodeEditor::~CodeEditor() {}

void CodeEditor::setCompleter(QCompleter *completer)
{
    if (m_completer)
        m_completer->disconnect(this);

    m_completer = completer;

    if (!m_completer)
        return;

    m_completer->setWidget(this);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);

    connect(m_completer, QOverload<const QString &>::of(&QCompleter::activated),
            this, &CodeEditor::insertCompletion);
}

QCompleter *CodeEditor::completer() const
{
    return m_completer;
}

void CodeEditor::insertCompletion(const QString &completion)
{
    if (m_completer->widget() != this)
        return;

    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    tc.removeSelectedText();
    tc.insertText(completion);
    setTextCursor(tc);
}

QString CodeEditor::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void CodeEditor::focusInEvent(QFocusEvent *e)
{
    if (m_completer)
        m_completer->setWidget(this);
    QPlainTextEdit::focusInEvent(e);
}

void CodeEditor::keyPressEvent(QKeyEvent *e)
{
    if (m_completer && m_completer->popup()->isVisible())
    {
        switch (e->key())
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return;
        default:
            break;
        }
    }

    // Auto-indent
    if (e->key() == Qt::Key_Tab)
    {
        insertPlainText("    "); // 4 spaces
        return;
    }

    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
    {
        QTextCursor cursor = textCursor();
        QString currentLine = cursor.block().text();

        // Count leading spaces
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

        // Add extra indent after {
        if (currentLine.trimmed().endsWith('{'))
            spaces += 2;

        QPlainTextEdit::keyPressEvent(e);

        // Insert spaces for indent
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

    bool isShortcut = (e->modifiers().testFlag(Qt::ControlModifier) &&
                       e->key() == Qt::Key_Space);

    if (!m_completer || !isShortcut)
        QPlainTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers().testFlag(Qt::ControlModifier) ||
                             e->modifiers().testFlag(Qt::ShiftModifier);

    if (!m_completer || (ctrlOrShift && e->text().isEmpty()))
        return;

    QString completionPrefix = textUnderCursor();

    if (!isShortcut && (e->text().isEmpty() || completionPrefix.length() < 2))
    {
        m_completer->popup()->hide();
        return;
    }

    if (completionPrefix != m_completer->completionPrefix())
    {
        m_completer->setCompletionPrefix(completionPrefix);
        m_completer->popup()->setCurrentIndex(
            m_completer->completionModel()->index(0, 0));
    }

    if (m_completer->completionCount() > 0)
    {
        QRect cr = cursorRect();
        cr.setWidth(m_completer->popup()->sizeHintForColumn(0) + m_completer->popup()->verticalScrollBar()->sizeHint().width());
        m_completer->complete(cr);
    }
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
