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

#include <sigfwd/sigfwd.hpp>
#include <boost/bind.hpp>

namespace {
void updateButtonEnabled(QAction* action, QAbstractButton* button)
{
    button->setEnabled(action->isEnabled());
}
}

void connectButtonToAction(QAbstractButton* button, QAction* action)
{
    QObject::connect(button, SIGNAL(clicked()), action, SLOT(trigger()));
    QObject::connect(action, SIGNAL(toggled(bool)), button, SLOT(setChecked(bool)));

    button->setEnabled(action->isEnabled());
    // ensure that the button is enabled/disabled whenever the QAction is enabled/disabled
    sigfwd::connect(action, SIGNAL(changed()),
                    boost::bind(&updateButtonEnabled, action, button));
}
