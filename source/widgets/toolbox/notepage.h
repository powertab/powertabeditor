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
  
#ifndef NOTEPAGE_H
#define NOTEPAGE_H

#include <QFrame>

#include <boost/shared_ptr.hpp>

class SkinManager;
class QPushButton;
class QAbstractButton;
class QGroupBox;
class QHBoxLayout;
class QVBoxLayout;
class PowerTabEditor;

class NotePage : public QFrame
{
    Q_OBJECT

public:
    NotePage(PowerTabEditor* mainWindow, boost::shared_ptr<SkinManager> skinManager, QFrame *parent = 0);

private:
    PowerTabEditor* mainWindow;
    QVBoxLayout *layout;

    QHBoxLayout *noteLayout;
    QHBoxLayout *restLayout;
    QHBoxLayout *rhythmLayout;
    QHBoxLayout *slideLegatoLayout;
    QHBoxLayout *vibratoTremoloLayout;

    QGroupBox *noteGroup;
    QGroupBox *restGroup;
    QGroupBox *rhythmGroup;
    QGroupBox *slideLegatoGroup;
    QGroupBox *vibratoTremoloGroup;

    // note group
    QPushButton *noteButton[7];

    // rest group
    QPushButton *restButton[7];

    // rhythm group
    QPushButton *dottedButton;
    QPushButton *doubleDottedButton;
    QPushButton *tieButton;
    QPushButton *groupingButton;
    QPushButton *fermataButton;

    // slide and legato group
    QPushButton *slideInBelowButton;
    QPushButton *slideInAboveButton;
    QPushButton *slideOutBelowButton;
    QPushButton *slideOutAboveButton;
    QPushButton *shiftSlideButton;
    QPushButton *legatoSlideButton;
    QPushButton *legatoButton;

    // vibrato and tremolo group
    QPushButton *vibratoButton;
    QPushButton *wideVibratoButton;
    QPushButton *bendButton;
    QPushButton *trillButton;
    QPushButton *tremoloButton;

    void createNoteButtons();
    void createRestButtons();
    void createRhythmButtons();
    void createSlideLegatoButtons();
    void createVibratoTremoloButtons();

    void initButton(QPushButton* button);

private slots:
    void resetDottedButtons();
};

#endif // NOTEPAGE_H
