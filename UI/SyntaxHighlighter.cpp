#include "SyntaxHighlighter.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    // Keyword format
    keywordFormat.setForeground(QColor(0, 0, 255));
    keywordFormat.setFontWeight(QFont::Bold);

    QStringList keywordPatterns = {
        "\\bint\\b",
        "\\bfloat\\b",
        "\\bdouble\\b",
        "\\bchar\\b",
        "\\blong\\b",
        "\\bvoid\\b",
        "\\breturn\\b",
        "\\bif\\b",
        "\\belse\\b",
        "\\bwhile\\b",
        "\\bfor\\b",
        "\\bconst\\b",
        "\\bstruct\\b",
        "\\btypedef\\b",
        "\\benum\\b",
        "\\bunion\\b",
        "\\bstatic\\b",
        "\\bextern\\b",
        "\\bauto\\b",
        "\\bregister\\b",
        "\\bsizeof\\b",
        "\\bbreak\\b",
        "\\bcontinue\\b",
        "\\bswitch\\b",
        "\\bcase\\b",
        "\\bdefault\\b",
        "\\bgoto\\b",
        "\\bdo\\b",
        "\\binclude\\b",
    };

    for (const QString &pattern : keywordPatterns)
    {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // Number format
    numberFormat.setForeground(QColor(0, 128, 128));
    rule.pattern = QRegularExpression("\\b[0-9]+\\.?[0-9]*\\b");
    rule.format = numberFormat;
    highlightingRules.append(rule);

    // Hex numbers
    rule.pattern = QRegularExpression("\\b0[xX][0-9a-fA-F]+\\b");
    rule.format = numberFormat;
    highlightingRules.append(rule);

    // Function format
    functionFormat.setForeground(QColor(128, 0, 128));
    functionFormat.setFontItalic(true);
    rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    // String format
    quotationFormat.setForeground(QColor(163, 21, 21));
    rule.pattern = QRegularExpression("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    // Character format
    rule.pattern = QRegularExpression("'.'");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    // Single-line comment format
    singleLineCommentFormat.setForeground(QColor(0, 128, 0));
    singleLineCommentFormat.setFontItalic(true);
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    // Multi-line comment format
    multiLineCommentFormat.setForeground(QColor(0, 128, 0));
    multiLineCommentFormat.setFontItalic(true);

    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");

    // Operator format
    operatorFormat.setForeground(QColor(128, 128, 0));
    operatorFormat.setFontWeight(QFont::Bold);
    QStringList operatorPatterns = {
        "\\+", "-", "\\*", "/", "%", "=", "==", "!=",
        "<", ">", "<=", ">=", "&&", "\\|\\|", "!",
        "\\+=", "-=", "\\*=", "/=", "%=", "\\+\\+", "--"};

    for (const QString &pattern : operatorPatterns)
    {
        rule.pattern = QRegularExpression(pattern);
        rule.format = operatorFormat;
        highlightingRules.append(rule);
    }
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    // Apply all rules
    for (const HighlightingRule &rule : qAsConst(highlightingRules))
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // Multi-line comments
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0)
    {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;

        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + match.capturedLength();
        }

        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}