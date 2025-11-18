#pragma once
#include <QListWidget>
#include <QKeyEvent>
#include <QApplication> 

class SuggestionWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit SuggestionWidget(QWidget *parent = nullptr)
        : QListWidget(parent)
    {
        setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setMaximumHeight(200);
        setStyleSheet(
            "QListWidget {"
            "  border: 1px solid #999;"
            "  border-radius: 4px;"
            "  background-color: white;"
            "  font-family: Consolas;"
            "  font-size: 11pt;"
            "}"
            "QListWidget::item {"
            "  padding: 4px 8px;"
            "  border-bottom: 1px solid #eee;"
            "}"
            "QListWidget::item:hover {"
            "  background-color: #e3f2fd;"
            "}"
            "QListWidget::item:selected {"
            "  background-color: #2196F3;"
            "  color: white;"
            "}");
    }
    bool event(QEvent *e) override
    {

        return QListWidget::event(e);
    }

signals:
    void suggestionSelected(const QString &text);

protected:
    void keyPressEvent(QKeyEvent *e) override
    {
        if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
        {
            if (currentItem())
            {
                emit suggestionSelected(currentItem()->text());
                hide();
            }
            e->accept(); 
            return;
        }
        else if (e->key() == Qt::Key_Escape)
        {
            hide();
            e->accept(); // Chấp nhận sự kiện
            return;
        }
        else if (e->key() == Qt::Key_Up || e->key() == Qt::Key_Down)
        {
            // Để QListWidget tự xử lý việc di chuyển lên xuống
            QListWidget::keyPressEvent(e); 
            return;
        }

        
        e->accept(); 

      
        if (parentWidget())
        {
            QApplication::sendEvent(parentWidget(), e);
        }
        
    }

    void mousePressEvent(QMouseEvent *e) override
    {
        QListWidget::mousePressEvent(e);
        if (currentItem())
        {
            emit suggestionSelected(currentItem()->text());
            hide();
        }
    }
};