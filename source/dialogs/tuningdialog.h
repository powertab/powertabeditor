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
  
#ifndef TUNINGDIALOG_H
#define TUNINGDIALOG_H

#include <QDialog>

#include <vector>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

class QComboBox;
class Guitar;
class Tuning;
class TuningDictionary;

namespace Ui {
    class TuningDialog;
}

class TuningDialog : public QDialog
{
    Q_OBJECT
public:
    TuningDialog(QWidget* parent, boost::shared_ptr<const Guitar> guitar,
                 boost::shared_ptr<TuningDictionary> tuningDictionary);
    ~TuningDialog();

    Tuning getNewTuning() const;
    uint8_t getNewCapo() const;

private slots:
    void toggleSharps(bool usesSharps);
    void updateEnabledStrings(int numStrings);
    void updateTuningDictionary(int numStrings);
    void loadPreset();

private:
    Ui::TuningDialog* ui;
    boost::shared_ptr<TuningDictionary> tuningDictionary;

    QStringList noteNames;
    std::vector<QComboBox*> stringSelectors;

    void initStringSelectors(const Tuning& currentTuning);
    void generateNoteNames(bool usesSharps);
};

#endif // TUNINGDIALOG_H
