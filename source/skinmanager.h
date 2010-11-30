#ifndef SKINMANAGER_H
#define SKINMANAGER_H

#include <QFile>
#include <QString>
#include <QTextStream>

class SkinManager
{
public:
	SkinManager(QString filename);

	QString getDocumentTabStyle();
	QString getToolboxTabStyle();
	QString getToolboxPageStyle();
	QString getMixerStyle();

private:
	QString documentTabStyle;
	QString toolboxTabStyle, toolboxPageStyle;
	QString mixerStyle;

	QString readSegment(QString skinname, QString filename);
};

#endif // SKINMANAGER_H
