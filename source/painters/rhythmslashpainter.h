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
  
#ifndef RHYTHMSLASHPAINTER_H
#define RHYTHMSLASHPAINTER_H

#include "painterbase.h"
#include <QFont>
#include <boost/shared_ptr.hpp>

class RhythmSlash;

class RhythmSlashPainter : public PainterBase
{
public:
    RhythmSlashPainter(boost::shared_ptr<const RhythmSlash> rhythmSlash);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    static const double STEM_OFFSET;
    static const double NOTE_HEAD_OFFSET;

private:
    boost::shared_ptr<const RhythmSlash> rhythmSlash;
    QFont musicFont;
};

#endif // RHYTHMSLASHPAINTER_H
