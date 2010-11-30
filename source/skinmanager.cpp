#include "skinmanager.h"

#include <QCoreApplication>

SkinManager::SkinManager()
{
}

bool SkinManager::openSkin(QString filename)
{
	QFile data;

	data.setFileName(QCoreApplication::applicationDirPath()+"/skins/"+filename+"/top_tab.txt");

	if(data.open(QFile::ReadOnly))
	{
		QTextStream styleIn(&data);
		topTabStyle = styleIn.readAll();
		data.close();
	}
	else
	{
		return false;
	}

	data.setFileName(QCoreApplication::applicationDirPath()+"/skins/"+filename+"/left_tab.txt");

	if(data.open(QFile::ReadOnly))
	{
		QTextStream styleIn(&data);
		leftTabStyle = styleIn.readAll();
		data.close();
	}
	else
	{
		return false;
	}

	data.setFileName(QCoreApplication::applicationDirPath()+"/skins/"+filename+"/toolbox_page.txt");

	if(data.open(QFile::ReadOnly))
	{
		QTextStream styleIn(&data);
		toolboxPageStyle = styleIn.readAll();
		data.close();
	}
	else
	{
		return false;
	}

	return true;
}

QString SkinManager::getTopTabStyle()
{
	return topTabStyle;
}

QString SkinManager::getLeftTabStyle()
{
	return leftTabStyle;
}

QString SkinManager::getToolboxPageStyle()
{
	return toolboxPageStyle;
}
