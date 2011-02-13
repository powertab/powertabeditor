#include "documentmanager.h"

#include <QMessageBox>
#include <QString>

#include <powertabdocument/powertabdocument.h>

DocumentManager::DocumentManager()
{
    currentDocumentIndex = -1;
}

DocumentManager::~DocumentManager()
{
    qDeleteAll(documentList.begin(), documentList.end());
}

PowerTabDocument* DocumentManager::getCurrentDocument()
{
    if (currentDocumentIndex < 0 || currentDocumentIndex >= documentList.size())
        return NULL;
    else
        return documentList.at(currentDocumentIndex);
}

void DocumentManager::removeDocument(int index)
{
    delete documentList[index];
    documentList.removeAt(index);
    currentDocumentIndex = 0;
}

bool DocumentManager::addDocument(const QString& fileName)
{
    // check that the document is not already open
    for (auto i = documentList.begin(); i != documentList.end(); ++i)
    {
        PowerTabDocument* doc = *i;
        if (doc->GetFileName() == fileName.toStdString())
        {
            currentDocumentIndex = i - documentList.begin();
            return false;
        }
    }

    // try to open the file
    PowerTabDocument* newDocument = new PowerTabDocument();
    bool success = newDocument->Load(fileName.toStdString());
    if (!success)
    {
        QMessageBox msgBox;
        msgBox.setText("Error opening file");
        msgBox.exec();
        return false;
    }
    else
    {
        documentList.append(newDocument);
        currentDocumentIndex = documentList.length() - 1;
    }

    return true;
}

void DocumentManager::setCurrentDocumentIndex(int index)
{
    currentDocumentIndex = index;
}

int DocumentManager::getCurrentDocumentIndex() const
{
    return currentDocumentIndex;
}

