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

#include <QSettings>
#include <app/settings.h>
#include <qstring.h>

DocumentManager::DocumentManager()
{
}

Document &DocumentManager::addDocument()
{
    myDocumentList.emplace_back(new Document());
    myCurrentIndex = static_cast<int>(myDocumentList.size()) - 1;
    return *myDocumentList.back();
}

Document &DocumentManager::addDefaultDocument()
{
    Document &doc = addDocument();
    Score &score = doc.getScore();
    QSettings settings;

    // Add an initial player and instrument.
    Player player;
    player.setDescription("Player 1");
    player.setTuning(settings.value(
            Settings::DEFAULT_INSTRUMENT_TUNING,
            QVariant::fromValue(Settings::DEFAULT_INSTRUMENT_TUNING_DEFAULT)
        ).value<Tuning>());
    score.insertPlayer(player);

    Instrument instrument;
    instrument.setDescription(settings.value(
            Settings::DEFAULT_INSTRUMENT_NAME,
            Settings::DEFAULT_INSTRUMENT_NAME_DEFAULT).toString().toStdString() + " 1");
    instrument.setMidiPreset(settings.value(Settings::DEFAULT_INSTRUMENT_PRESET,
            Settings::DEFAULT_INSTRUMENT_PRESET_DEFAULT).toInt());
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

Document &DocumentManager::getCurrentDocument()
{
    if (!myCurrentIndex)
        throw std::logic_error("No documents are currently open");

    return *myDocumentList.at(*myCurrentIndex);
}

Document &DocumentManager::getDocument(int i)
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
    return myCurrentIndex.is_initialized();
}

void DocumentManager::setCurrentDocumentIndex(int index)
{
    if (index < 0)
    {
        Q_ASSERT(myDocumentList.empty());
        myCurrentIndex.reset();
    }
    else
        myCurrentIndex = index;
}

int DocumentManager::getCurrentDocumentIndex() const
{
    return *myCurrentIndex;
}

size_t DocumentManager::getDocumentListSize() const 
{
	return myDocumentList.size();
}

int DocumentManager::containsDocument(const std::string& filepath)
{
	for (int i = 0; i < getDocumentListSize(); ++i)
	{
		Document& doc = getDocument(i);
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
    return myFilename.is_initialized();
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

const Caret &Document::getCaret() const
{
    return myCaret;
}

Caret &Document::getCaret()
{
    return myCaret;
}
