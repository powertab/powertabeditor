#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

class QTabWidget;
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QComboBox;

// Dialog to allow the user to modify general editor-wide settings

class MIDITab : public QWidget
{
    Q_OBJECT
public:
    explicit MIDITab(QWidget *parent = 0);

    QVBoxLayout *root_layout;
    QHBoxLayout *bottom_line;
    QLabel		*label;
    QComboBox	*box;

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
    //Barline* barLine;
    //QSpinBox* repeatCount;
    //QComboBox* barLineType;

private slots:
    //void disableRepeatCount(int newBarlineType);
    void accept();
    void reject();
};

#endif // PREFERENCESDIALOG_H
