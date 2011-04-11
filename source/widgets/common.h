#ifndef COMMON_H
#define COMMON_H

// Contains general code that is used by multiple widgets

#include <QLabel>

class QAbstractButton;
class QAction;

void connectButtonToAction(QAbstractButton* button, QAction* action);

// Provides a subclass of QLabel that emits signals when it is clicked
class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    ClickableLabel(QWidget* parent = 0) :
        QLabel(parent)
    {
    }

protected:
    void mouseReleaseEvent(QMouseEvent*)
    {
        emit clicked();
    }

    void mouseDoubleClickEvent(QMouseEvent*)
    {
        emit doubleClicked();
    }

signals:
    void clicked();
    void doubleClicked();
};

#endif // COMMON_H
