#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

class QTabWidget;
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QComboBox;
class QCheckBox;
class QFormLayout;
class QSpinBox;

// Dialog to allow the user to modify general editor-wide settings

class MIDITab : public QWidget
{
    Q_OBJECT
public:
    explicit MIDITab(QWidget *parent = 0);

    QVBoxLayout* rootLayout;
    QHBoxLayout* bottomLine;
    QFormLayout* formLayout;
    QComboBox*	midiPort;
    QCheckBox* metronomeEnabled;
    QSpinBox* vibratoStrength;
    QSpinBox* wideVibratoStrength;

signals:

public slots:

};

class PreferencesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PreferencesDialog(QWidget *parent = 0);

private:
    MIDITab* midiTab;
    QTabWidget* tabWidget;

private slots:
    void accept();
};

#endif // PREFERENCESDIALOG_H
