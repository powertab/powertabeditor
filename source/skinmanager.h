#ifndef SKINMANAGER_H
#define SKINMANAGER_H

#include <QFile>
#include <QString>
#include <QTextStream>

class SkinManager
{
public:
    SkinManager();

	bool openSkin(QString filename);
	QString getTopTabStyle();
	QString getLeftTabStyle();
	QString getToolboxPageStyle();

private:
	QString topTabStyle, leftTabStyle, toolboxPageStyle;
};

#endif // SKINMANAGER_H
