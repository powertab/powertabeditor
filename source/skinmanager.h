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

private:
	QString topTabStyle, leftTabStyle;
};

#endif // SKINMANAGER_H
