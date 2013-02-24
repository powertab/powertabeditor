/*
  * Copyright (C) 2011 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
  
#include "documentmanager.h"

#include <QMessageBox>
#include <QString>

#include <powertabdocument/powertabdocument.h>
#include <app/common.h>

using boost::shared_ptr;

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

void DocumentManager::addDocument(boost::shared_ptr<PowerTabDocument> doc)
{
    documentList.push_back(doc);
    currentDocumentIndex = documentList.size() - 1;
}

void DocumentManager::setCurrentDocumentIndex(int index)
{
    index = Common::clamp(index, -1, static_cast<int>(documentList.size() - 1));
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

