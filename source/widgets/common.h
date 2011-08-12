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
