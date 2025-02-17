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

#include <app/settings.h>
#include <app/settingsmanager.h>

DocumentManager::DocumentManager()
{
}

Document &DocumentManager::addDocument(
        const SettingsManager &settings_manager)
{
    myDocumentList.emplace_back(std::make_unique<Document>());
    myCurrentIndex = static_cast<int>(myDocumentList.size()) - 1;

    auto settings = settings_manager.getReadHandle();
    myDocumentList.back()->getViewOptions().setZoom(settings->get(Settings::LastZoomLevel));

    return *myDocumentList.back();
}

Document &DocumentManager::addDefaultDocument(
    const SettingsManager &settings_manager)
{
    Document &doc = addDocument(settings_manager);
    Score &score = doc.getScore();

    auto settings = settings_manager.getReadHandle();

    // Add an initial player and instrument.
    Player player;
    player.setDescription("Player 1");
    player.setTuning(settings->get(Settings::DefaultTuning));
    score.insertPlayer(player);

    Instrument instrument;
    instrument.setDescription(settings->get(Settings::DefaultInstrumentName) +
                              " 1");
    instrument.setMidiPreset(settings->get(Settings::DefaultInstrumentPreset));
    score.insertInstrument(instrument);

    ScoreUtils::addStandardFilters(score);

    // Add an initial staff, player change, and tempo marker.
    System system;
    system.insertStaff(Staff(player.getTuning().getStringCount()));

    PlayerChange initialPlayers;
    initialPlayers.insertActivePlayer(0, ActivePlayer(0, 0));
    system.insertPlayerChange(initialPlayers);

    TempoMarker tempo;
    tempo.setDescription("Moderately");
    system.insertTempoMarker(tempo);

    score.insertSystem(system);

    return doc;
}

Document &
DocumentManager::getCurrentDocument()
{
    if (!myCurrentIndex)
        throw std::logic_error("No documents are currently open");

    return *myDocumentList.at(*myCurrentIndex);
}

Document &
DocumentManager::getDocument(int i)
{
    return *myDocumentList.at(i);
}

const Document &
DocumentManager::getDocument(int i) const
{
    return *myDocumentList.at(i);
}

void DocumentManager::removeDocument(int index)
{
    myDocumentList.erase(myDocumentList.begin() + index);

    const int n = static_cast<int>(myDocumentList.size());
    if (myDocumentList.empty())
        myCurrentIndex.reset();
    else if (n >= *myCurrentIndex)
        myCurrentIndex = n - 1;
}

bool DocumentManager::hasOpenDocuments() const
{
    return myCurrentIndex.has_value();
}

void DocumentManager::setCurrentDocumentIndex(int index)
{
    if (index < 0)
    {
        assert(myDocumentList.empty());
        myCurrentIndex.reset();
    }
    else
        myCurrentIndex = index;
}

int DocumentManager::getCurrentDocumentIndex() const
{
    return *myCurrentIndex;
}

int
DocumentManager::getNumDocuments() const
{
    return static_cast<int>(myDocumentList.size());
}

int DocumentManager::findDocument(const Document::PathType &filepath)
{
    for (int i = 0, n = getNumDocuments(); i < n; ++i)
    {
        Document& doc = getDocument(i);
        
        if (!doc.hasFilename())
            continue;

        if (filepath == doc.getFilename())
            return i;
        
    }
    return -1;
}

Document::Document()
    : myCaret(myScore, myViewOptions)
{
}

bool Document::hasFilename() const
{
    return myFilename.has_value();
}

const Document::PathType &Document::getFilename() const
{
    return *myFilename;
}

void Document::setFilename(const PathType &filename)
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

void Document::validateViewOptions()
{
    myViewOptions.ensureValid(myScore);
}

const Caret &Document::getCaret() const
{
    return myCaret;
}

Caret &Document::getCaret()
{
    return myCaret;
}
