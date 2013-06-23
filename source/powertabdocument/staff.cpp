/////////////////////////////////////////////////////////////////////////////
// Name:            staff.cpp
// Purpose:         Stores and renders a staff
// Author:          Brad Larsen
// Modified by:
// Created:         Dec 16, 2004
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "staff.h"

#include "position.h"
#include "system.h"
#include "barline.h"
#include "powertabfileheader.h"             // Needed for file version constants
#include "tuning.h"                         // Needed for IsValidStringCount
#include "keysignature.h"
#include "powertabinputstream.h"
#include "powertaboutputstream.h"
#include "generalmidi.h"

#include <numeric> // partial_sum
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/unordered_map.hpp>

// Default Constants
const uint8_t Staff::DEFAULT_DATA                                        = (uint8_t)((DEFAULT_CLEF << 4) | DEFAULT_TABLATURE_STAFF_TYPE);
const uint8_t Staff::DEFAULT_CLEF                                        = TREBLE_CLEF;
const uint8_t Staff::DEFAULT_TABLATURE_STAFF_TYPE                        = 6;
const uint8_t Staff::DEFAULT_STANDARD_NOTATION_STAFF_ABOVE_SPACING       = 9;
const uint8_t Staff::DEFAULT_STANDARD_NOTATION_STAFF_BELOW_SPACING       = 9;
const uint8_t Staff::DEFAULT_SYMBOL_SPACING                              = 0;
const uint8_t Staff::DEFAULT_TABLATURE_STAFF_BELOW_SPACING               = 0;
const uint8_t Staff::STD_NOTATION_LINE_SPACING = 7;
const uint8_t Staff::STD_NOTATION_STAFF_TYPE = 5;
const uint8_t Staff::STAFF_BORDER_SPACING = 10;
const uint8_t Staff::TAB_SYMBOL_HEIGHT = 10;

// Clef Constants
const uint8_t Staff::TREBLE_CLEF                         = 0;
const uint8_t Staff::BASS_CLEF                           = 1;

// Tablature Staff Type Constants
const uint8_t Staff::MIN_TABULATURE_STAFF_TYPE = Tuning::MIN_STRING_COUNT;
const uint8_t Staff::MAX_TABULATURE_STAFF_TYPE = Tuning::MAX_STRING_COUNT;

// maps notes to their position on the staff (relative to the top line), used for Staff::GetNoteLocation
// this is for treble clef - it is adjusted for bass clef as necessary
namespace {
const boost::unordered_map<char, int8_t> notePositions = boost::assign::map_list_of
                    ('F', 0) ('E', 1) ('D', 2) ('C', 3) ('B', -3) ('A', -2) ('G', -1);
}

/// Default Constructor
Staff::Staff() :
    m_data(DEFAULT_DATA),
    m_standardNotationStaffAboveSpacing(DEFAULT_STANDARD_NOTATION_STAFF_ABOVE_SPACING),
    m_standardNotationStaffBelowSpacing(DEFAULT_STANDARD_NOTATION_STAFF_BELOW_SPACING),
    m_symbolSpacing(DEFAULT_SYMBOL_SPACING),
    m_tablatureStaffBelowSpacing(DEFAULT_TABLATURE_STAFF_BELOW_SPACING),
    m_isShown(true)
{
}

/// Primary Constructor
/// @param tablatureStaffType The type of tablature staff to set (number of strings)
/// @param clef Type of clef to set (see CLEF constants)
Staff::Staff(uint8_t tablatureStaffType, uint8_t clef) :
    m_data(DEFAULT_DATA),
    m_standardNotationStaffAboveSpacing(DEFAULT_STANDARD_NOTATION_STAFF_ABOVE_SPACING),
    m_standardNotationStaffBelowSpacing(DEFAULT_STANDARD_NOTATION_STAFF_BELOW_SPACING),
    m_symbolSpacing(DEFAULT_SYMBOL_SPACING),
    m_tablatureStaffBelowSpacing(DEFAULT_TABLATURE_STAFF_BELOW_SPACING),
    m_isShown(true)
{
    SetClef(clef);
    SetTablatureStaffType(tablatureStaffType);
}

/// Copy Constructor
Staff::Staff(const Staff& staff) :
    PowerTabObject(),
    m_data(DEFAULT_DATA),
    m_standardNotationStaffAboveSpacing(DEFAULT_STANDARD_NOTATION_STAFF_ABOVE_SPACING),
    m_standardNotationStaffBelowSpacing(DEFAULT_STANDARD_NOTATION_STAFF_BELOW_SPACING),
    m_symbolSpacing(DEFAULT_SYMBOL_SPACING),
    m_tablatureStaffBelowSpacing(DEFAULT_TABLATURE_STAFF_BELOW_SPACING)
{
    *this = staff;
}

Staff::~Staff()
{
    for (size_t i = 0; i < positionArrays.size(); i++)
    {
        std::vector<Position*>& positionArray = positionArrays[i];
        for (size_t j = 0; j < positionArray.size(); j++)
        {
            delete positionArray[j];
        }
    }
}

const Staff& Staff::operator=(const Staff& staff)
{
    if (this != &staff)
    {
        m_data = staff.m_data;
        m_standardNotationStaffAboveSpacing =
            staff.m_standardNotationStaffAboveSpacing;
        m_standardNotationStaffBelowSpacing =
            staff.m_standardNotationStaffBelowSpacing;
        m_symbolSpacing = staff.m_symbolSpacing;
        m_tablatureStaffBelowSpacing = staff.m_tablatureStaffBelowSpacing;
        m_isShown = staff.m_isShown;

        for (size_t i = 0; i < staff.positionArrays.size(); i++)
        {
            positionArrays[i].reserve(staff.positionArrays[i].size());
            // clone each position array
            std::transform(staff.positionArrays[i].begin(),
                           staff.positionArrays[i].end(),
                           std::back_inserter(positionArrays[i]),
                           std::mem_fun(&Position::CloneObject));
        }
    }
    return *this;
}

Staff* Staff::CloneObject() const
{
    return new Staff(*this);
}

struct ComparePositionPointers
{
    bool operator()(Position* pos1, Position* pos2)
    {
        return *pos1 == *pos2;
    }
};

bool Staff::operator==(const Staff& staff) const
{
    // compare each position in each voice
    for (size_t i = 0; i < staff.positionArrays.size(); i++)
    {
        if (staff.positionArrays[i].size() != positionArrays[i].size())
        {
            return false;
        }

        if (!std::equal(positionArrays[i].begin(), positionArrays[i].end(),
                        staff.positionArrays[i].begin(), ComparePositionPointers()))
        {
            return false;
        }
    }

    return ((m_data == staff.m_data) &&
            (m_standardNotationStaffAboveSpacing == staff.m_standardNotationStaffAboveSpacing) &&
            (m_standardNotationStaffBelowSpacing == staff.m_standardNotationStaffBelowSpacing) &&
            (m_symbolSpacing == staff.m_symbolSpacing) &&
            (m_isShown == staff.m_isShown) &&
            (m_tablatureStaffBelowSpacing == staff.m_tablatureStaffBelowSpacing));
}

bool Staff::operator!=(const Staff& staff) const
{
    return (!operator==(staff));
}

// Serialize Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool Staff::Serialize(PowerTabOutputStream& stream) const
{
    //------Last Checked------//
    // - Jan 5, 2005
    stream << m_data << m_standardNotationStaffAboveSpacing <<
        m_standardNotationStaffBelowSpacing << m_symbolSpacing <<
        m_tablatureStaffBelowSpacing;
    PTB_CHECK_THAT(stream.CheckState(), false);

    // TODO - should we serialize the number of voices??
    for (size_t i = 0; i < positionArrays.size(); i++)
    {
        stream.WriteVector(positionArrays[i]);
        PTB_CHECK_THAT(stream.CheckState(), false);
    }

    return stream.CheckState();
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool Staff::Deserialize(PowerTabInputStream& stream, uint16_t version)
{
    stream >> m_data >> m_standardNotationStaffAboveSpacing >>
              m_standardNotationStaffBelowSpacing >> m_symbolSpacing >>
              m_tablatureStaffBelowSpacing;

    for (size_t i = 0; i < positionArrays.size(); i++)
    {
        stream.ReadVector(positionArrays[i], version);
    }

    return true;
}

/// Sets the clef used on the standard notation staff
/// @param clef Clef to set
/// @return True if the clef was set, false if not
bool Staff::SetClef(uint8_t clef)
{
    //------Last Checked------//
    // - Jan 5, 2005
    PTB_CHECK_THAT(clef <= 1, false);

    m_data &= ~clefMask;
    m_data |= (uint8_t)(clef << 4);

    return (true);
}

/// Sets the tablature staff type
/// @param type Tablature staff type to set
/// @return True if the tablature staff type was set, false if not
bool Staff::SetTablatureStaffType(uint8_t type)
{
    PTB_CHECK_THAT(Tuning::IsValidStringCount(type), false);
    PTB_CHECK_THAT(IsValidTablatureStaffType(type), false);

    m_data &= ~tablatureStaffTypeMask;
    m_data |= type;

    return (true);
}

/// Calculates the height of the staff
int Staff::GetHeight() const
{
    if (!IsShown())
    {
        return 0;
    }
    else
    {
        return  GetStandardNotationStaffAboveSpacing() + GetStandardNotationStaffBelowSpacing() + GetSymbolSpacing() +
                GetTablatureStaffBelowSpacing() + STD_NOTATION_LINE_SPACING * (STD_NOTATION_STAFF_TYPE - 1) +
                (GetTablatureStaffType() - 1) * 9 + 4 * STAFF_BORDER_SPACING; // TODO - pass in the tab line separation as a parameter
    }
}

/// Determines if a Clef is valid
/// @param clef Clef to validate
/// @return True if the clef is valid, false if not
bool Staff::IsValidClef(uint8_t clef)
{
    return clef == TREBLE_CLEF || clef == BASS_CLEF;
}

/// Gets the clef used on the standard notation staff
/// @return The clef used on the standard notation staff
uint8_t Staff::GetClef() const
{
    return (m_data & clefMask) >> 4;
}

/// Determines if a Tablature Staff Type is valid. In addition to checking
/// whether the number of strings is in range, it will check if all notes in
/// the staff are within the range.
/// @param type Tablature staff type to validate.
/// @return True if the tablature staff type is valid, false if not.
bool Staff::IsValidTablatureStaffType(uint8_t type) const
{
    if (type < MIN_TABULATURE_STAFF_TYPE || type > MAX_TABULATURE_STAFF_TYPE)
    {
        return false;
    }

    for (size_t i = 0; i < positionArrays.size(); ++i)
    {
        const std::vector<Position*>& voice = positionArrays[i];
        for (size_t j = 0; j < voice.size(); ++j)
        {
            const Position* pos = voice.at(j);
            for (size_t m = 0; m < pos->GetNoteCount(); ++m)
            {
                const Note* note = pos->GetNote(m);
                if (note->GetString() >= type)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

/// Gets the tablature staff type (3-7 strings)
/// @return The tablature staff type
uint8_t Staff::GetTablatureStaffType() const
{
    return m_data & tablatureStaffTypeMask;
}

/// Sets the amount of spacing above the standard notation staff
/// @param spacing Spacing to set
void Staff::SetStandardNotationStaffAboveSpacing(uint8_t spacing)
{
    m_standardNotationStaffAboveSpacing = spacing;
}

/// Gets the amount of spacing above the standard notation staff
/// @return The amount of spacing above the standard notation staff
uint8_t Staff::GetStandardNotationStaffAboveSpacing() const
{
    return m_standardNotationStaffAboveSpacing;
}

/// Sets the amount of spacing below the standard notation staff
/// @param spacing Spacing to set
void Staff::SetStandardNotationStaffBelowSpacing(uint8_t spacing)
{
    m_standardNotationStaffBelowSpacing = spacing;
}

/// Gets the amount of spacing below the standard notation staff
/// @return The amount of spacing below the standard notation staff
uint8_t Staff::GetStandardNotationStaffBelowSpacing() const
{
    return m_standardNotationStaffBelowSpacing;
}

/// Sets the amount of spacing used by symbols in between the standard
/// notation and tablature staves
/// @param spacing Spacing to set
void Staff::SetSymbolSpacing(uint8_t spacing)
{
    m_symbolSpacing = spacing;
}

/// Gets the amount of spacing used by symbols in between the standard
/// notation and tablature staves
/// @return The amount of spacing used by symbols in between the standard
/// notation and tablature staves
uint8_t Staff::GetSymbolSpacing() const
{
    return m_symbolSpacing;
}

/// Sets the amount of spacing below the tablature staff
/// @param spacing Spacing to set
void Staff::SetTablatureStaffBelowSpacing(uint8_t spacing)
{
    m_tablatureStaffBelowSpacing = spacing;
}

/// Gets the amount of spacing below the tablature staff
/// @return The amount of spacing below the tablature staff
uint8_t Staff::GetTablatureStaffBelowSpacing() const
{
    return m_tablatureStaffBelowSpacing;
}

/// Determines if a voice is valid
/// @param voice Voice to validate
/// @return True if the voice is valid, false if not
bool Staff::IsValidVoice(uint32_t voice)
{
    return voice < NUM_STAFF_VOICES;
}

/// Determines if a position index is valid
/// @param voice Voice the position belongs to
/// @param index position index to validate
/// @return True if the position index is valid, false if not
bool Staff::IsValidPositionIndex(uint32_t voice, uint32_t index) const
{
    PTB_CHECK_THAT(IsValidVoice(voice), false);
    return (index < GetPositionCount(voice));
}

/// Gets the number of positions in the staff
/// @param voice Voice of the positions to get the count of
/// @throw std::out_of_range if the voice is invalid
size_t Staff::GetPositionCount(uint32_t voice) const
{
    if (!IsValidVoice(voice))
        throw std::out_of_range("Invalid voice");

    return positionArrays[voice].size();
}

/// Gets the nth position in the staff
/// @param voice Voice the position belongs to
/// @param index Index of the position to get
/// @throw std::out_of_range if the voice or index are invalid
Position* Staff::GetPosition(uint32_t voice, uint32_t index) const
{
    if (!IsValidVoice(voice))
        throw std::out_of_range("Invalid voice");

    return positionArrays[voice].at(index);
}

/// Finds the Position object at the given position index & voice in the staff
/// @return The position object, or NULL if not found
/// @throw std::out_of_range If the voice is invalid
Position* Staff::GetPositionByPosition(uint32_t voice, uint32_t index) const
{
    if (!IsValidVoice(voice))
        throw std::out_of_range("Invalid voice");

    const std::vector<Position*>& positionArray = positionArrays.at(voice);

    for (size_t i = 0; i < positionArray.size(); i++)
    {
        if (positionArray[i]->GetPosition() == index)
        {
            return positionArray[i];
        }
    }

    return NULL;
}

/// Finds the position index of the next position
/// @throw std::out_of_range if the position does not exist in this staff, or if the voice is invalid
uint32_t Staff::GetIndexOfNextPosition(uint32_t voice, boost::shared_ptr<const System> system,
                                     const Position* position) const
{
    if (!IsValidVoice(voice))
        throw std::out_of_range("Invalid voice");

    const std::vector<Position*>& positionArray = positionArrays.at(voice);

    std::vector<Position*>::const_iterator location = std::find(positionArray.begin(),
                                                                positionArray.end(), position);

    if (location == positionArray.end())
        throw std::out_of_range("Position not in system");

    std::advance(location, 1);

    if (location == positionArray.end())
    {
        return system->GetPositionCount() - 1;
    }
    else
    {
        return (*location)->GetPosition();
    }
}

// Returns true if the given position is the only Position object in its bar
bool Staff::IsOnlyPositionInBar(const Position* position,
                                boost::shared_ptr<const System> system,
                                uint32_t voice) const
{
    std::vector<System::BarlineConstPtr> barlines;
    system->GetBarlines(barlines);

    System::BarlineConstPtr prevBarline = system->GetPrecedingBarline(position->GetPosition());

    std::vector<System::BarlineConstPtr>::const_iterator startBar = std::find(barlines.begin(),
                                                                              barlines.end(), prevBarline);
    std::vector<System::BarlineConstPtr>::const_iterator endBar = startBar + 1;

    for (uint32_t i = (*startBar)->GetPosition() + 1; i < (*endBar)->GetPosition(); i++)
    {
        const Position* pos = GetPositionByPosition(voice, i);
        if (pos != NULL && pos != position)
        {
            return false;
        }
    }

    return true;
}

bool Staff::CanHammerOn(const Position* position, const Note* note,
                        uint32_t voice) const
{
    // In order to perform a hammer-on, the note must be lower than the next
    return CompareWithNote(NextNote, position, note, voice,
                           std::less<uint8_t>());
}

bool Staff::CanPullOff(const Position* position, const Note* note,
                       uint32_t voice) const
{
    // In order to perform a pull-off, the note must be higher than the next
    return CompareWithNote(NextNote, position, note, voice,
                           std::greater<uint8_t>());
}

// Figures out if the given note can be set as tied
// The previous position in the staff must contain a Note at the same string & fret
bool Staff::CanTieNote(const Position* position, const Note* note,
                       uint32_t voice) const
{
    // In order to tie, the note must be the same as the previous
    return CompareWithNote(PrevNote, position, note, voice,
                           std::equal_to<uint8_t>());
}

/// Determines if we can slide from the given note to the next note (shift or legato slides)
/// The next note must exist and be a different fret number
bool Staff::CanSlideBetweenNotes(const Position* position, const Note* note,
                                 uint32_t voice) const
{
    return CompareWithNote(NextNote, position, note, voice,
                           std::not_equal_to<uint8_t>());
}

namespace 
{
    // Tests a predicate member function against a pointer to an object of type T
    template<typename T>
    struct TestPredicatePtr
    {
        typedef bool (T::*Predicate)() const;
        
        TestPredicatePtr(T* object) : object(object) {}
        
        bool operator()(Predicate pred)
        {
            return (object->*pred)();
        }
        
    private:
        T* object;
    };
}

/// Calculates the beaming for notes that are located between the two given barlines
void Staff::CalculateBeamingForBar(boost::shared_ptr<const Barline> startBar,
                                   boost::shared_ptr<const Barline> endBar)
{
    for (uint32_t i = 0; i < NUM_STAFF_VOICES; ++i)
    {
        CalculateBeamingForVoice(startBar, endBar, i);
    }
}

void Staff::CalculateBeamingForVoice(boost::shared_ptr<const Barline> startBar,
                                     boost::shared_ptr<const Barline> endBar,
                                     uint32_t voice)
{
    // Get the positions in betwen the two bars
    std::vector<Position*> positions;
    GetPositionsInRange(positions, voice, startBar->GetPosition(),
                        endBar->GetPosition() - 1);

    const TimeSignature& timeSig = startBar->GetTimeSignature();

    // Get the beam group patterns from the time signature
    std::vector<uint8_t> beamGroupPatterns(4, 0);

    timeSig.GetBeamingPattern(beamGroupPatterns[0], beamGroupPatterns[1],
                              beamGroupPatterns[2], beamGroupPatterns[3]);

    // Remove any beam group patterns of size 0 (not set)
    beamGroupPatterns.erase(std::remove(beamGroupPatterns.begin(),
                                        beamGroupPatterns.end(), 0),
                            beamGroupPatterns.end());

    // Create a list of the durations for each position
    std::vector<double> durations(positions.size());
    std::transform(positions.begin(), positions.end(),
                   durations.begin(), std::mem_fun(&Position::GetDuration));
    // Convert the duration list to a list of partial sums of the durations
    // (i.e. timestamps relative to the beginning of the bar)
    std::partial_sum(durations.begin(), durations.end(), durations.begin());

    double groupBeginTime = 0;
    std::vector<uint8_t>::const_iterator pattern = beamGroupPatterns.begin();
    std::vector<double>::iterator groupStart = durations.begin();
    std::vector<double>::iterator groupEnd = durations.begin();

    while (groupEnd != durations.end())
    {
        // find where the end of the current beaming pattern group will be
        const double groupEndTime = *pattern * 0.5 + groupBeginTime;

        // get the range of positions between the start of the beaming pattern group and the end
        groupStart = std::lower_bound(groupEnd, durations.end(), groupBeginTime);
        groupEnd = std::upper_bound(groupStart, durations.end(), groupEndTime);

        // get the corresponding positions, and calculate the beaming for this pattern group
        std::vector<Position*> positionGroup(positions.begin() + (groupStart - durations.begin()),
                                             positions.begin() + (groupEnd - durations.begin()));

        CalculateBeamingForGroup(positionGroup);

        // Move on to the next beaming pattern, looping around if necessary
        ++pattern;
        if (pattern == beamGroupPatterns.end())
            pattern = beamGroupPatterns.begin();

        groupBeginTime = groupEndTime;
    }
}

/// Sets the beaming properties for a group of consecutive notes
/// (i.e. notes that are part of a beaming pattern group)
void Staff::CalculateBeamingForGroup(std::vector<Position*>& positions)
{
    // Rests and notes greater than eighth notes will break apart a beam group,
    // so we need to find all of the subgroups of consecutive positions that can be
    // beamed, and then create beaming groups with those notes

    std::vector<Position*>::iterator beamableGroupStart = positions.begin();
    std::vector<Position*>::iterator beamableGroupEnd = positions.begin();

    // Clear all existing beaming information
    std::for_each(positions.begin(), positions.end(), std::mem_fun(&Position::ClearBeam));

    // find all subgroups of beamable notes (i.e. notes that aren't quarter notes, rests, etc)
    while (beamableGroupStart != positions.end())
    {
        // find the next range of consecutive positions that are beamable
        beamableGroupStart = std::find_if(beamableGroupEnd, positions.end(),
                                          std::mem_fun(&Position::IsBeamable));

        beamableGroupEnd = std::find_if(beamableGroupStart, positions.end(),
                                        std::not1(std::mem_fun(&Position::IsBeamable)));

        for (std::vector<Position*>::iterator i = beamableGroupStart;
             i != beamableGroupEnd; ++i)
        {
            Position* currentPos = *i;

            if (i == beamableGroupStart)
            {
                currentPos->SetPreviousBeamDurationType(0);
                continue;
            }

            // set the previous beam duration
            Position* prevPos = *(i - 1);
            uint8_t prevDuration = prevPos->GetDurationType();

            // previous beam duration only applies for consecutive notes with the same duration type
            if (currentPos->GetDurationType() != prevDuration)
            {
                prevDuration = 8;
            }
            else
            {
                // clear fractional beams for the previous position, since we will connect it to the current note
                prevPos->SetFractionalLeftBeam(false);
                prevPos->SetFractionalRightBeam(false);
            }

            currentPos->SetPreviousBeamDurationType(prevDuration);

            // set any fractional beams
            if (currentPos->GetDurationType() > prevPos->GetDurationType())
            {
                currentPos->SetFractionalRightBeam();
            }
            else if (currentPos->GetDurationType() < prevPos->GetDurationType())
            {
                // a previously set beam takes precedence
                if (!prevPos->HasFractionalRightBeam())
                    prevPos->SetFractionalLeftBeam();
            }
        }

        if (beamableGroupStart != positions.end())
        {
            // set start/end beams
            (*beamableGroupStart)->SetBeamStart();
            (*(beamableGroupEnd - 1))->SetBeamEnd();
        }
    }
}

namespace {
struct ComparePositionLower
{
    bool operator()(const Position* pos, size_t location)
    {
        return pos->GetPosition() < location;
    }
};
}

/// Gets all of the positions within the given range (inclusive)
/// @param Output parameter, to store the positions that are in the range
void Staff::GetPositionsInRange(std::vector<Position*>& positionsInRange, uint32_t voice, size_t startPos, size_t endPos) const
{
    if (!IsValidVoice(voice))
        throw std::out_of_range("Invalid voice");

    assert(startPos <= endPos);

    const std::vector<Position*>& positionArray = positionArrays.at(voice);

    std::vector<Position*>::const_iterator low = std::lower_bound(positionArray.begin(),
                                                                  positionArray.end(),
                                                                  startPos,
                                                                  ComparePositionLower());

    positionsInRange.clear();
    while (low != positionArray.end())
    {
        if ((*low)->GetPosition() <= endPos)
        {
            positionsInRange.push_back(*low);
            ++low;
        }
        else
        {
            return;
        }
    }
}

/// Returns the last position in the staff, regardless of voice (returns NULL if no positions exist)
Position* Staff::GetLastPosition() const
{
    std::vector<Position*> lastPositions;

    // construct a list of the last positions for each voice
    for (size_t i = 0; i < positionArrays.size(); i++)
    {
        if (!positionArrays[i].empty())
            lastPositions.push_back(positionArrays[i].back());
    }

    if (lastPositions.empty())
        return NULL;

    // return the position with the largest position index
    return *std::max_element(lastPositions.begin(), lastPositions.end(),
                            boost::bind(std::less<uint32_t>(),
                                 boost::bind(&Position::GetPosition, _1),
                                 boost::bind(&Position::GetPosition, _2)
                                 ));
}

/// Returns the number of steps (frets) between the given note and the next note on the string
/// @throws std::logic_error If there is no note at the same string for the next position
int8_t Staff::GetSlideSteps(const Position* position, const Note* note,
                            uint32_t voice) const
{
    Note* nextNote = GetAdjacentNoteOnString(Staff::NextNote, position, note,
                                             voice);

    if (!nextNote)
        throw std::logic_error("The next position does not have a note on the same string.");

    return nextNote->GetFretNumber() - note->GetFretNumber();
}

Note* Staff::GetAdjacentNoteOnString(SearchDirection searchDirection,
                                     const Position *position, const Note *note,
                                     uint32_t voice) const
{
    if (!IsValidVoice(voice))
        throw std::out_of_range("Invalid voice");

    const std::vector<Position*>& positionArray = positionArrays[voice];

    // find where the position is within the staff
    std::vector<Position*>::const_iterator location = std::find(positionArray.begin(),
                                                                positionArray.end(), position);

    // if position was not found, we cannot compare it to the next one
    if (location == positionArray.end())
    {
        return NULL;
    }

    // check that the new location is still valid
    int newIndex = location - positionArray.begin() + searchDirection;
    if (newIndex < 0 || newIndex >= (int)positionArray.size())
    {
        return NULL;
    }

    std::advance(location, searchDirection);

    Position* nextPosition = *location;
    Note* nextNote = nextPosition->GetNoteByString(note->GetString());

    return nextNote;
}

void Staff::UpdateTabNumber(Position* position, Note* note, uint32_t voice,
                            uint8_t fretNumber)
{
    note->SetFretNumber(fretNumber);

    // Update hammerons, ties, etc for adjacent notes.
    UpdateAdjacentNotes(position, voice, note->GetString());
}

void Staff::ShiftTabNumber(Position* position, Note* note, uint32_t voice,
                           bool shiftUp, const Tuning& tuning)
{
    const uint32_t prevString = note->GetString();
    Position::ShiftType type = shiftUp ? Position::SHIFT_UP :
                                         Position::SHIFT_DOWN;
    position->ShiftTabNumber(note, type, tuning);

    UpdateAdjacentNotes(position, voice, prevString);
    UpdateAdjacentNotes(position, voice, note->GetString());
}

void Staff::UpdateAdjacentNotes(Position* position, uint32_t voice,
                                uint32_t string)
{
    Note* note = position->GetNoteByString(string);
    const size_t index = GetIndexOfPosition(voice, position);

    if (index != 0)
    {
        Position* prevPos = GetPosition(voice, index - 1);
        Note* prevNote = prevPos->GetNoteByString(string);
        UpdateNote(prevPos, prevNote, note, voice);
    }

    if (index + 1 != GetPositionCount(voice))
    {
        Position* nextPos = GetPosition(voice, index + 1);
        Note* nextNote = nextPos->GetNoteByString(string);
        UpdateNote(nextPos, note, nextNote, voice);
    }
}

/// Ensures that all hammerons, pulloffs, and slides are valid for the first note,
/// and ensures that any ties for the second note are valid. Either of the notes
/// may be NULL if they do not exist.
void Staff::UpdateNote(Position *prevPosition, Note *previousNote,
                       Note *nextNote, uint32_t voice)
{
    if (previousNote)
    {
        const bool canPull = CanPullOff(prevPosition, previousNote, voice);
        const bool canHammer = CanHammerOn(prevPosition, previousNote, voice);

        if (previousNote->HasPullOff() && !canPull)
        {
            previousNote->SetPullOff(false);
            if (canHammer)
                previousNote->SetHammerOn(true);
        }
        else if (previousNote->HasHammerOn() && !canHammer)
        {
            previousNote->SetHammerOn(false);
            if (canPull)
                previousNote->SetPullOff(true);
        }

        // need to check slides
        uint8_t slideType;
        int8_t slideSteps;
        if (previousNote->GetSlideOutOf(slideType, slideSteps))
        {
            // if the note used to slide but no longer can then remove slide
            if (!CanSlideBetweenNotes(prevPosition, previousNote, voice))
            {
                previousNote->SetSlideOutOf(Note::slideOutOfNone, 0);
            }
            else
            {
                int8_t newSteps = GetSlideSteps(prevPosition, previousNote,
                                                voice);
                previousNote->ClearSlideOutOf();
                previousNote->SetSlideOutOf(slideType, newSteps);
            }
        }
    }

    if (nextNote && nextNote->IsTied() &&
            (!previousNote ||
             nextNote->GetFretNumber() != previousNote->GetFretNumber()))
    {
        nextNote->SetTied(false);
    }
}

struct OrderByPosition
{
    inline bool operator() (const Position* pos1, const Position* pos2)
    {
        return pos1->GetPosition() < pos2->GetPosition();
    }
};

/// Insert a position into the given voice
/// @throw std::out_of_range If the voice is not valid
bool Staff::InsertPosition(uint32_t voice, Position *position)
{
    if (!IsValidVoice(voice))
        throw std::out_of_range("Invalid voice");

    // check that the new position won't overlap with existing positions
    if (GetPositionByPosition(voice, position->GetPosition()) != NULL)
        return false;

    std::vector<Position*>& positionArray = positionArrays[voice];

    // add the position and re-sort by position index
    positionArray.push_back(position);
    std::sort(positionArray.begin(), positionArray.end(), OrderByPosition());

    return true;
}

/// Removes the given position from the specified voice
bool Staff::RemovePosition(uint32_t voice, uint32_t index)
{
    if (!IsValidVoice(voice))
        throw std::out_of_range("Invalid voice");

    // get the position object
    Position* pos = GetPositionByPosition(voice, index);
    PTB_CHECK_THAT(pos != NULL, false);

    // get the iterator to it (for erasing from the array)
    std::vector<Position*>& positionArray = positionArrays[voice];
    std::vector<Position*>::iterator location = std::find(positionArray.begin(), positionArray.end(), pos);
    PTB_CHECK_THAT(location != positionArray.end(), false);

    // remove it
    positionArray.erase(location);
    return true;
}

/// Finds the location of the note, relative to the top line of the staff
/// e.g. for a treble clef, F4 -> 0, E4 -> 1, G4 -> - 1
int Staff::GetNoteLocation(const Note* note, const KeySignature& activeKeySig,
                           const Tuning& tuning) const
{
    const uint8_t pitch = note->GetPitch(tuning, true);
    
    const std::string noteText = midi::GetMidiNoteText(pitch, activeKeySig.IsMinorKey(),
            activeKeySig.UsesSharps() || activeKeySig.HasNoKeyAccidentals(),
            activeKeySig.NumberOfAccidentals());

    // find the position of the note, ignoring accidentals (i.e. C# -> C)
    int y = notePositions.find(noteText[0])->second;
    
    if (GetClef() == BASS_CLEF) // adjust for bass clef, where A is the note at the top line
    {
        y += 2;
    }
    
    const uint8_t TOP_NOTE = (GetClef() == TREBLE_CLEF) ? midi::MIDI_NOTE_F4 : midi::MIDI_NOTE_A2;
    
    // add octave shifts
    y += 7 * (midi::GetMidiNoteOctave(TOP_NOTE) -
              midi::GetMidiNoteOctave(pitch, noteText[0])) +
            7 * note->GetOctaveOffset();
    
    return y;
}

void Staff::RemoveNote(uint32_t voice, uint32_t position, uint32_t string)
{
    Position* pos = GetPositionByPosition(voice, position);
    assert(pos != NULL);

    pos->RemoveNote(string);
    UpdateAdjacentNotes(pos, voice, string);
}

/// Computes the appropriate clef (treble or bass) depending on the notes present in the system
void Staff::CalculateClef(const Tuning& tuning)
{
    BOOST_FOREACH(const std::vector<Position*>& posArray, positionArrays)
    {
        BOOST_FOREACH(const Position* position, posArray)
        {
            for (uint8_t i = 0; i < position->GetNoteCount(); i++)
            {
                if (position->GetNote(i)->GetPitch(tuning, true) <= midi::MIDI_NOTE_A1)
                {
                    SetClef(BASS_CLEF);
                    return;
                }
            }
        }
    }

    SetClef(TREBLE_CLEF);
    return;
}

/// Returns the location of a position in the position array (e.g. 5th position from beginning of staff)
size_t Staff::GetIndexOfPosition(uint32_t voice, const Position *position) const
{
    const std::vector<Position*>& posArray = positionArrays.at(voice);
    assert(std::find(posArray.begin(), posArray.end(), position) != posArray.end());
    return std::find(posArray.begin(), posArray.end(), position) - posArray.begin();
}

/// Returns whether or not the staff is visible
bool Staff::IsShown() const
{
    return m_isShown;
}

/// Sets the staff to be visible
void Staff::SetShown(bool set)
{
    m_isShown = set;
}
