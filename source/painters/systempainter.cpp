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
  
#include "systempainter.h"

#include <powertabdocument/system.h>

#include <QPainter>

SystemPainter::SystemPainter(boost::shared_ptr<const System> system) :
    system(system)
{
    init();
}

void SystemPainter::init()
{
    Rect rect = system->GetRect();

    bounds = QRectF(0, 0, rect.GetWidth(), rect.GetHeight());

    path.addRect(bounds);
}

void SystemPainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setOpacity(0.5);
    painter->setPen(QPen(QBrush(QColor(0,0,0)), 0.5));

    painter->drawPath(path);
}
