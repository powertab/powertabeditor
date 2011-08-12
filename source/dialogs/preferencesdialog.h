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
  
#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

class QTabWidget;
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QComboBox;
class QCheckBox;
class QFormLayout;
class QSpinBox;

// Dialog to allow the user to modify general editor-wide settings

class MIDITab : public QWidget
{
    Q_OBJECT
public:
    explicit MIDITab(QWidget *parent = 0);

    QVBoxLayout* rootLayout;
    QHBoxLayout* bottomLine;
    QFormLayout* formLayout;
    QComboBox*	midiPort;
    QCheckBox* metronomeEnabled;
    QSpinBox* vibratoStrength;
    QSpinBox* wideVibratoStrength;

signals:

public slots:

};

class PreferencesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PreferencesDialog(QWidget *parent = 0);

private:
    MIDITab* midiTab;
    QTabWidget* tabWidget;

private slots:
    void accept();
};

#endif // PREFERENCESDIALOG_H
