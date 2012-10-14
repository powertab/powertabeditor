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
  
#ifndef CHORDNAMEDIALOG_H
#define CHORDNAMEDIALOG_H

#include <QDialog>
#include <QVector>

#include <powertabdocument/chordname.h>

class QLineEdit;
class QCheckBox;
class QPushButton;
class QListWidget;
class QButtonGroup;
class QSignalMapper;

class ChordNameDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ChordNameDialog(QWidget* parent, ChordName* chord);

protected:
    ChordName chordName;
    ChordName* originalChordName;

    QLineEdit* preview;
    QCheckBox* noChord;
    QCheckBox* usesBrackets;
    QPushButton* toggleSharps;
    QPushButton* toggleFlats;
    QSignalMapper* sharpFlatMapper;
    QPushButton* toggleBassSharps;
    QPushButton* toggleBassFlats;
    QSignalMapper* sharpFlatBassMapper;

    QListWidget* formulaList;

    QCheckBox* add2;
    QCheckBox* add4;
    QCheckBox* add6;
    QCheckBox* add9;
    QCheckBox* add11;

    QCheckBox* extended9th;
    QCheckBox* extended11th;
    QCheckBox* extended13th;

    QCheckBox* flatted5th;
    QCheckBox* raised5th;
    QCheckBox* flatted9th;
    QCheckBox* raised9th;
    QCheckBox* raised11th;
    QCheckBox* flatted13th;
    QCheckBox* suspended2nd;
    QCheckBox* suspended4th;

    QButtonGroup* tonicKey;
    QVector<QPushButton*> tonicKeyOptions;
    QButtonGroup* bassKey;
    QVector<QPushButton*> bassKeyOptions;

    void init();
    void setIfChecked(QCheckBox* checkBox, quint16 flag);
    void initKeyOptions(QVector<QPushButton*>& buttons, QButtonGroup* group);
    inline void initCheckBox(QCheckBox* checkBox, const QString& text);
    inline void setNote(quint8 key, quint8 variation, bool bass);
    void updateNote(int key, bool bass);

protected slots:
    void updateData();
    void updateTonicNote(int key);
    void updateBassNote(int key);
    void toggleSharpFlat(QWidget* button);

public slots:
    void accept();
};

#endif // CHORDNAMEDIALOG_H
