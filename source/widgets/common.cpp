#include "common.h"

#include <QAction>
#include <QAbstractButton>

void connectButtonToAction(QAbstractButton* button, QAction* action)
{
    QObject::connect(button, SIGNAL(clicked()), action, SLOT(trigger()));
    QObject::connect(action, SIGNAL(toggled(bool)), button, SLOT(setChecked(bool)));
}
