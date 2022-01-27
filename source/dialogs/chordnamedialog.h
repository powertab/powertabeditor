/*
 * Copyright (C) 2013 Cameron White
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

#ifndef DIALOGS_CHORDNAMEDIALOG_H
#define DIALOGS_CHORDNAMEDIALOG_H

#include <QDialog>
#include <score/chordname.h>
#include <vector>

namespace Ui {
class ChordNameDialog;
}

class QAbstractButton;
class QButtonGroup;
class QCheckBox;

class ChordNameDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ChordNameDialog(QWidget *parent, const ChordName &initial_chord,
                             std::vector<ChordName> available_chords);
    ~ChordNameDialog();

    const ChordName &getChordName() const;

private slots:
    void updateState();
    void onTonicVariationClicked(QAbstractButton *clickedButton);
    void onBassVariationClicked(QAbstractButton *clickedButton);
    void onTonicChanged();
    
private:
    void initCheckBox(QCheckBox *checkbox);
    void setToChord(const ChordName &chord);

    Ui::ChordNameDialog *ui;
    QButtonGroup *myTonicVariations;
    QButtonGroup *myBassVariations;
    QButtonGroup *myTonicKeys;
    QButtonGroup *myBassKeys;

    ChordName myChord;
    std::vector<ChordName> myScoreChords;
};

#endif
