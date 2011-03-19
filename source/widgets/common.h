#ifndef COMMON_H
#define COMMON_H

// Contains general code that is used by multiple widgets

class QAbstractButton;
class QAction;

void connectButtonToAction(QAbstractButton* button, QAction* action);

#endif // COMMON_H
