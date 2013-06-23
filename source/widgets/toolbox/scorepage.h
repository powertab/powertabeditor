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
  
#ifndef SCOREPAGE_H
#define SCOREPAGE_H

#include <QFrame>

#include <boost/shared_ptr.hpp>

class SkinManager;
class QPushButton;
class QGroupBox;
class QHBoxLayout;
class QVBoxLayout;
class PowerTabEditor;

class ScorePage : public QFrame
{
    Q_OBJECT

public:
    ScorePage(PowerTabEditor* mainWindow, boost::shared_ptr<SkinManager> skinManager, QFrame *parent = 0);

private:
    PowerTabEditor* mainWindow;

    QVBoxLayout *layout;

    QHBoxLayout *songLayout;
    QHBoxLayout *sectionLayout;
    //QHBoxLayout *staffLayout;
    QHBoxLayout *flowDynamicsLayout;

    QGroupBox *songGroup;
    QGroupBox *sectionGroup;
    //QGroupBox *staffGroup;
    QGroupBox *flowDynamicsGroup;

    // song group
    QPushButton *addInstrumentButton;
    QPushButton *addPercussionButton;
    QPushButton *increaseHeightButton;
    QPushButton *decreaseHeightButton;

    // section group
    QPushButton *insertSectionBeforeButton;
    QPushButton *insertSectionAfterButton;
    QPushButton *removeSectionButton;
    QPushButton *addBarButton;
    QPushButton *increaseWidthButton;
    QPushButton *decreaseWidthButton;
    QPushButton *justifyButton;

    // staff group
    //QPushButton *

    // flow and dynamics group
    QPushButton *addRehearsalSignButton;
    QPushButton *addDirectionButton;
    QPushButton *addTempoMarkerButton;
    QPushButton *addVolumeMarkerButton;

    void createSongButtons();
    void createSectionButtons();
    //void createStaffButtons();
    void createFlowDynamicsButtons();
};

#endif // SCOREPAGE_H
