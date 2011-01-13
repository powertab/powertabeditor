#ifndef CHORDNAMEDIALOG_H
#define CHORDNAMEDIALOG_H

#include <QDialog>
#include <QVector>

#include <powertabdocument/chordname.h>

class QLineEdit;
class QCheckBox;
class QPushButton;
class QListWidget;
class QButtonGroup;
class QSignalMapper;

class ChordNameDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ChordNameDialog(ChordName* chord, QWidget *parent = 0);

protected:
    ChordName chordName;
    ChordName* originalChordName;

    QLineEdit* preview;
    QCheckBox* noChord;
    QCheckBox* usesBrackets;
    QPushButton* toggleSharps;
    QPushButton* toggleFlats;
    QSignalMapper* sharpFlatMapper;
    QPushButton* toggleBassSharps;
    QPushButton* toggleBassFlats;
    QSignalMapper* sharpFlatBassMapper;

    QListWidget* formulaList;

    QCheckBox* add2;
    QCheckBox* add4;
    QCheckBox* add6;
    QCheckBox* add9;
    QCheckBox* add11;

    QCheckBox* extended9th;
    QCheckBox* extended11th;
    QCheckBox* extended13th;

    QCheckBox* flatted5th;
    QCheckBox* raised5th;
    QCheckBox* flatted9th;
    QCheckBox* raised9th;
    QCheckBox* raised11th;
    QCheckBox* flatted13th;
    QCheckBox* suspended2nd;
    QCheckBox* suspended4th;

    QButtonGroup* tonicKey;
    QVector<QPushButton*> tonicKeyOptions;
    QButtonGroup* bassKey;
    QVector<QPushButton*> bassKeyOptions;

    void init();
    void setIfChecked(QCheckBox* checkBox, quint16 flag);
    void initKeyOptions(QVector<QPushButton*>& buttons, QButtonGroup* group);
    inline void initCheckBox(QCheckBox* checkBox, const QString& text);
    inline void setNote(quint8 key, quint8 variation, bool bass);
    void updateNote(int key, bool bass);

protected slots:
    void updateData();
    void updateTonicNote(int key);
    void updateBassNote(int key);
    void toggleSharpFlat(QWidget* button);

public slots:
    void accept();
    void reject();
};

#endif // CHORDNAMEDIALOG_H
