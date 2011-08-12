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
  
#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include <vector>
#include <memory>

class QString;
class PowerTabDocument;

/// Class for managing opened documents
class DocumentManager
{
public:
    DocumentManager();

    bool addDocument(const QString& fileName);
    bool addImportedDocument(std::shared_ptr<PowerTabDocument> doc);

    void createDocument();
    void removeDocument(int index);
    std::shared_ptr<PowerTabDocument> getCurrentDocument() const;
    void setCurrentDocumentIndex(int index);
    int getCurrentDocumentIndex() const;

private:
    std::vector<std::shared_ptr<PowerTabDocument> > documentList;
    int currentDocumentIndex;
};

#endif // DOCUMENTMANAGER_H
