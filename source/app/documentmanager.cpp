#include "documentmanager.h"

#include <QMessageBox>
#include <QString>

#include <powertabdocument/powertabdocument.h>
#include <app/common.h>

using std::shared_ptr;

DocumentManager::DocumentManager()
{
    currentDocumentIndex = -1;
}

shared_ptr<PowerTabDocument> DocumentManager::getCurrentDocument() const
{
    if (currentDocumentIndex == -1)
    {
        return shared_ptr<PowerTabDocument>();
    }
    else
    {
        return documentList.at(currentDocumentIndex);
    }
}

void DocumentManager::removeDocument(int index)
{
    documentList.erase(documentList.begin() + index);
    currentDocumentIndex = 0;
}

bool DocumentManager::addDocument(const QString& fileName)
{
    // check that the document is not already open
    for (auto i = documentList.begin(); i != documentList.end(); ++i)
    {
        if ((*i)->GetFileName() == fileName.toStdString())
        {
            currentDocumentIndex = i - documentList.begin();
            return false;
        }
    }

    // try to open the file
    shared_ptr<PowerTabDocument> newDocument(new PowerTabDocument);
    const bool success = newDocument->Load(fileName.toStdString());
    if (!success)
    {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Error opening file"));
        msgBox.exec();
        return false;
    }
    else
    {
        documentList.push_back(newDocument);
        currentDocumentIndex = documentList.size() - 1;
    }

    return true;
}

void DocumentManager::setCurrentDocumentIndex(int index)
{
    index = clamp(index, -1, static_cast<int>(documentList.size() - 1));
    currentDocumentIndex = index;
}

int DocumentManager::getCurrentDocumentIndex() const
{
    return currentDocumentIndex;
}

/// creates a new (blank) document
void DocumentManager::createDocument()
{
    shared_ptr<PowerTabDocument> doc(new PowerTabDocument);
    doc->SetFileName("Untitled.ptb");
    doc->Init();
    documentList.push_back(doc);
    currentDocumentIndex = documentList.size() - 1;
}

