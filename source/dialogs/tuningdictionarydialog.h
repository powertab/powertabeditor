/*
  * Copyright (C) 2012 Cameron White
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

#ifndef DIALOGS_TUNINGDICTIONARYDIALOG_H
#define DIALOGS_TUNINGDICTIONARYDIALOG_H

#include <QDialog>
#include <app/tuningdictionary.h>

namespace Ui {
class TuningDictionaryDialog;
}

class QTreeWidgetItem;
class Tuning;

class TuningDictionaryDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TuningDictionaryDialog(QWidget *parent,
                                    TuningDictionary &dictionary);
    ~TuningDictionaryDialog();

private slots:
    void onNumStringsChanged(int);
    void onNewTuning();
    void onDeleteTuning();
    void onCurrentTuningChanged(QTreeWidgetItem *, QTreeWidgetItem *);
    void onEditTuning();
    
private:
    void onTuningModified();
    TuningDictionary::Entry *selectedTuning() const;

    Ui::TuningDictionaryDialog *ui;
    TuningDictionary &myDictionary;
};

#endif
