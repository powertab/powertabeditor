#ifndef TUNINGDIALOG_H
#define TUNINGDIALOG_H

#include <QDialog>

#include <vector>
#include <memory>

class Tuning;
class QComboBox;
class QLineEdit;
class QCheckBox;
class QSpinBox;
class Guitar;

class TuningDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TuningDialog(std::shared_ptr<Guitar> guitar, QWidget *parent = 0);

public slots:
    void accept();

private slots:
    void toggleSharps(bool usesSharps);
    void updateEnabledStrings(int numStrings);

private:
    std::shared_ptr<Guitar> guitar;
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
