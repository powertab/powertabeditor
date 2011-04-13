#ifndef TUNINGDIALOG_H
#define TUNINGDIALOG_H

#include <QDialog>

#include <vector>

class Tuning;
class QComboBox;
class QLineEdit;
class QCheckBox;
class QSpinBox;

class TuningDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TuningDialog(const Tuning& tuning, QWidget *parent = 0);

public slots:
    void accept();
    void reject();

private slots:
    void toggleSharps(bool usesSharps);
    void updateEnabledStrings(int numStrings);

private:
    const Tuning& tuning;
    QStringList noteNames;
    
    QLineEdit* tuningNameEditor;
    QCheckBox* usesSharpsSelector;
    QSpinBox* numStringsSelector;
    QSpinBox* notationOffsetSelector;

    std::vector<QComboBox*> stringSelectors;

    void initStringSelectors();
    void generateNoteNames(bool usesSharps);
};

#endif // TUNINGDIALOG_H
