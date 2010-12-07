#include "skinmanager.h"

#include <QCoreApplication>
#include <QFile>
#include <QTextStream>

SkinManager::SkinManager(QString filename)
{
    documentTabStyle = readSegment(filename,"document_tab.txt");

    toolboxTabStyle = readSegment(filename,"toolbox_tab.txt");
    toolboxPageStyle = readSegment(filename,"toolbox_page.txt");

    mixerStyle = readSegment(filename,"mixer.txt");
}

QString SkinManager::getDocumentTabStyle()
{
    return documentTabStyle;
}

QString SkinManager::getToolboxTabStyle()
{
    return toolboxTabStyle;
}

QString SkinManager::getToolboxPageStyle()
{
    return toolboxPageStyle;
}

QString SkinManager::getMixerStyle()
{
    return mixerStyle;
}

QString SkinManager::readSegment(QString skinname, QString filename)
{
    QString out;
    QFile data;

    data.setFileName(QCoreApplication::applicationDirPath()+"/skins/"+skinname+"/"+filename);

    if(data.open(QFile::ReadOnly))
    {
        QTextStream styleIn(&data);
        out = styleIn.readAll();
        data.close();
    }

    return out;
}
