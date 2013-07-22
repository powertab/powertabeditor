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
  
#ifndef DIALOGS_KEYSIGNATUREDIALOG_H
#define DIALOGS_KEYSIGNATUREDIALOG_H

#include <QDialog>
#include <score/keysignature.h>

namespace Ui {
    class KeySignatureDialog;
}

class KeySignatureDialog : public QDialog
{
    Q_OBJECT

public:
    KeySignatureDialog(QWidget *parent, const KeySignature &currentKey);
    ~KeySignatureDialog();

    KeySignature getNewKey() const;

private slots:
    /// Populate the list of key types, depending on whether we are using
    /// major or minor keys.
    void populateKeyTypes(KeySignature::KeyType type);

    /// After the user makes a modification, set the key signature to be visible.
    /// This is only done for the first modification in case the user doesn't
    /// want to show the key signature.
    void handleModification();

private:
    Ui::KeySignatureDialog *ui;
    bool myIsModified;
    const KeySignature myPreviousKey;
};

#endif // KEYSIGNATUREDIALOG_H
