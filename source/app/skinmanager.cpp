#include "skinmanager.h"

#include <QCoreApplication>
#include <QFile>
#include <QTextStream>

SkinManager::SkinManager(const QString& filename)
{
    documentTabStyle = readSegment(filename,"document_tab.txt");

    toolboxTabStyle = readSegment(filename,"toolbox_tab.txt");
    toolboxPageStyle = readSegment(filename,"toolbox_page.txt");

    mixerStyle = readSegment(filename,"mixer.txt");
}

QString SkinManager::getDocumentTabStyle() const
{
    return documentTabStyle;
}

QString SkinManager::getToolboxTabStyle() const
{
    return toolboxTabStyle;
}

QString SkinManager::getToolboxPageStyle() const
{
    return toolboxPageStyle;
}

QString SkinManager::getMixerStyle() const
{
    return mixerStyle;
}

QString SkinManager::readSegment(const QString& skinname, const QString& filename)
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
