#ifndef KEYSIGNATUREDIALOG_H
#define KEYSIGNATUREDIALOG_H

#include <QDialog>
class QComboBox;
class QRadioButton;
class QCheckBox;

#include <powertabdocument/keysignature.h>

class KeySignatureDialog : public QDialog
{
    Q_OBJECT

public:
    KeySignatureDialog(const KeySignature& key);

    KeySignature getNewKey() const;

private slots:
    void toggleKeyType(uint8_t type);
    void toggleVisible(bool visible);
    void setKeyAccidentals(int accidentals);

private:
    const KeySignature& originalKey;
    KeySignature newKey;

    QComboBox* keyList;
    QRadioButton* majorKey;
    QRadioButton* minorKey;
    QCheckBox* visibilityToggle;

    void init();
    void populateKeyTypes();
};

#endif // KEYSIGNATUREDIALOG_H
