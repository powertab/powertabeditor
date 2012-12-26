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

#ifndef TUNINGDICTIONARYDIALOG_H
#define TUNINGDICTIONARYDIALOG_H

#include <QDialog>
#include <boost/shared_ptr.hpp>

namespace Ui {
class TuningDictionaryDialog;
}

class QTreeWidgetItem;
class Tuning;
class TuningDictionary;

class TuningDictionaryDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TuningDictionaryDialog(
            boost::shared_ptr<TuningDictionary> tuningDictionary,
            QWidget *parent);
    ~TuningDictionaryDialog();

private slots:
    void onNumStringsChanged(int);
    void onNewTuning();
    void onDeleteTuning();
    void onCurrentTuningChanged(QTreeWidgetItem *, QTreeWidgetItem *);
    void onEditTuning();
    
private:
    void onTuningModified();
    boost::shared_ptr<Tuning> selectedTuning() const;

    Ui::TuningDictionaryDialog *ui;
    boost::shared_ptr<TuningDictionary> tuningDictionary;
};

#endif // TUNINGDICTIONARYDIALOG_H
