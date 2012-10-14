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

#ifndef FILEINFORMATIONDIALOG_H
#define FILEINFORMATIONDIALOG_H

#include <QDialog>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <powertabdocument/powertabfileheader.h>

class PowerTabDocument;
class QAbstractButton;

namespace Ui {
class FileInformationDialog;
}

class FileInformationDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FileInformationDialog(QWidget* parent,
                                   boost::shared_ptr<PowerTabDocument> doc);
    ~FileInformationDialog();

    PowerTabFileHeader getNewFileHeader() const;
    
private:
    Ui::FileInformationDialog *ui;
    const uint8_t contentType;

    QString getFileVersionString(const PowerTabFileHeader& header);

private slots:
    void handleSongTypeButtonClick(QAbstractButton* button);
    void handleReleaseTypeChanged(int index);
    void handleAuthorTypeChanged(bool traditional);
};

#endif // FILEINFORMATIONDIALOG_H
