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
#include <QSettings>
#include <QString>

#include <app/common.h>
#include <app/settings.h>

DocumentManager::DocumentManager()
{
}

Document &DocumentManager::addDocument()
{
    myDocumentList.push_back(new Document());

    // TODO - set up a default document.
#if 0
    QSettings settings;
    Guitar defaultGuitar;
    defaultGuitar.SetDescription(settings.value(
            Settings::DEFAULT_INSTRUMENT_NAME,
            Settings::DEFAULT_INSTRUMENT_NAME_DEFAULT).toString().toStdString());
    defaultGuitar.SetPreset(settings.value(Settings::DEFAULT_INSTRUMENT_PRESET,
            Settings::DEFAULT_INSTRUMENT_PRESET_DEFAULT).toInt());
    defaultGuitar.SetTuning(settings.value(
            Settings::DEFAULT_INSTRUMENT_TUNING,
            QVariant::fromValue(Settings::DEFAULT_INSTRUMENT_TUNING_DEFAULT)).value<Tuning>());
    doc->Init(defaultGuitar);
#endif

    myCurrentIndex = myDocumentList.size() - 1;
    return myDocumentList.back();
}

boost::optional<Document &> DocumentManager::getCurrentDocument()
{
    if (myCurrentIndex)
        return boost::none;
    else
        return myDocumentList.at(*myCurrentIndex);
}

void DocumentManager::removeDocument(int index)
{
    myDocumentList.erase(myDocumentList.begin() + index);

    if (myDocumentList.empty())
        myCurrentIndex = boost::none;
    else if (myDocumentList.size() >= myCurrentIndex)
        myCurrentIndex = myDocumentList.size() - 1;
}

bool DocumentManager::hasOpenDocuments() const
{
    return myCurrentIndex;
}

void DocumentManager::setCurrentDocumentIndex(int index)
{
    if (myDocumentList.empty())
        throw std::logic_error("No open documents");

    myCurrentIndex = index;
}

int DocumentManager::getCurrentDocumentIndex() const
{
    return *myCurrentIndex;
}


Document::Document()
{
}

bool Document::hasFilename() const
{
    return myFilename;
}

const std::string &Document::getFilename() const
{
    return *myFilename;
}

void Document::setFilename(const std::string &filename)
{
    myFilename = filename;
}

const Score &Document::getScore() const
{
    return myScore;
}

Score &Document::getScore()
{
    return myScore;
}
