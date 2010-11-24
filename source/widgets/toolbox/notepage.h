#ifndef NOTEPAGE_H
#define NOTEPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

class NotePage : public QWidget
{
	Q_OBJECT

public:
	NotePage(QWidget *parent = 0);

private:
	QVBoxLayout *layout;

	QHBoxLayout *noteLayout;
	QHBoxLayout *restLayout;
	QHBoxLayout *rhythmLayout;

	QGroupBox *noteGroup;
	QGroupBox *restGroup;
	QGroupBox *rhythmGroup;

	QPushButton *noteButton[7];
	QPushButton *restButton[7];

	QPushButton *dottedButton;
	QPushButton *doubledottedButton;
	QPushButton *tieButton;

	void createNoteButtons();
	void createRestButtons();
	void createOtherButtons();

private slots:
	void resetNoteAndRestButtons();
	void resetDottedButtons();
};

#endif // NOTEPAGE_H
