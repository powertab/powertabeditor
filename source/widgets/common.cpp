/*
  * Copyright (C) 2011 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
  
#include "common.h"

#include <QAction>
#include <QAbstractButton>

void connectButtonToAction(QAbstractButton *button, const QAction *action)
{
    QObject::connect(button, &QAbstractButton::clicked, action,
                     &QAction::trigger);
    QObject::connect(action, &QAction::toggled, button,
                     &QAbstractButton::setChecked);

    button->setEnabled(action->isEnabled());
    // ensure that the button is enabled/disabled whenever the QAction is enabled/disabled
    QObject::connect(action, &QAction::changed, [=]() {
        button->setEnabled(action->isEnabled());
    });
}
