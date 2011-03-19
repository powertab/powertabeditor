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
#include "powertabfileheader.h"             // Needed for file version constants
#include "tuning.h"                         // Needed for IsValidStringCount

#include <numeric> // partial_sum
#include <stdexcept>
#include <functional>

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
// Note: If these values change, update the Tuning constants for string count
const uint8_t Staff::MIN_TABLATURE_STAFF_TYPE            = 3;
const uint8_t Staff::MAX_TABLATURE_STAFF_TYPE            = 7;

/// Default Constructor
Staff::Staff() :
    m_data(DEFAULT_DATA),
    m_standardNotationStaffAboveSpacing(DEFAULT_STANDARD_NOTATION_STAFF_ABOVE_SPACING),
    m_standardNotationStaffBelowSpacing(DEFAULT_STANDARD_NOTATION_STAFF_BELOW_SPACING),
    m_symbolSpacing(DEFAULT_SYMBOL_SPACING),
    m_tablatureStaffBelowSpacing(DEFAULT_TABLATURE_STAFF_BELOW_SPACING)
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
    m_tablatureStaffBelowSpacing(DEFAULT_TABLATURE_STAFF_BELOW_SPACING)
{
    SetClef(clef);
    SetTablatureStaffType(tablatureStaffType);
}

/// Copy Constructor
Staff::Staff(const Staff& staff) :
    m_data(DEFAULT_DATA),
    m_standardNotationStaffAboveSpacing(DEFAULT_STANDARD_NOTATION_STAFF_ABOVE_SPACING),
    m_standardNotationStaffBelowSpacing(DEFAULT_STANDARD_NOTATION_STAFF_BELOW_SPACING),
    m_symbolSpacing(DEFAULT_SYMBOL_SPACING),
    m_tablatureStaffBelowSpacing(DEFAULT_TABLATURE_STAFF_BELOW_SPACING)
{
    *this = staff;
}

/// Destructor
Staff::~Staff()
{
    for (auto i = positionArrays.begin(); i != positionArrays.end(); ++i)
    {
        for (auto j = i->begin(); j != i->end(); ++j)
        {
            delete *j;
        }
        i->clear();
    }
}

/// Assignment Operator
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

        positionArrays = staff.positionArrays;
    }
    return *this;
}

Staff* Staff::CloneObject() const
{
    Staff* newStaff = new Staff;

    newStaff->m_data = m_data;
    newStaff->m_standardNotationStaffAboveSpacing = m_standardNotationStaffAboveSpacing;
    newStaff->m_standardNotationStaffBelowSpacing = m_standardNotationStaffBelowSpacing;
    newStaff->m_symbolSpacing = m_symbolSpacing;
    newStaff->m_tablatureStaffBelowSpacing = m_tablatureStaffBelowSpacing;

    for (size_t i = 0; i < positionArrays.size(); i++)
    {
        // clone each position array into the new staff
        std::transform(positionArrays[i].begin(),
                       positionArrays[i].end(),
                       std::back_inserter(newStaff->positionArrays[i]),
                       std::mem_fun(&Position::CloneObject));
    }

    return newStaff;
}

/// Equality Operator
bool Staff::operator==(const Staff& staff) const
{
    //------Last Checked------//
    // - Jan 5, 2005
    return (
        (m_data == staff.m_data) &&
        (m_standardNotationStaffAboveSpacing ==
            staff.m_standardNotationStaffAboveSpacing) &&
        (m_standardNotationStaffBelowSpacing ==
            staff.m_standardNotationStaffBelowSpacing) &&
        (m_symbolSpacing == staff.m_symbolSpacing) &&
        (m_tablatureStaffBelowSpacing == staff.m_tablatureStaffBelowSpacing) &&
        (positionArrays == staff.positionArrays)
    );
}

/// Inequality Operator
bool Staff::operator!=(const Staff& staff) const
{
    //------Last Checked------//
    // - Jan 5, 2005
    return (!operator==(staff));
}

// Serialize Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool Staff::Serialize(PowerTabOutputStream& stream)
{
    //------Last Checked------//
    // - Jan 5, 2005
    stream << m_data << m_standardNotationStaffAboveSpacing <<
        m_standardNotationStaffBelowSpacing << m_symbolSpacing <<
        m_tablatureStaffBelowSpacing;
    CHECK_THAT(stream.CheckState(), false);

    // TODO - should we serialize the number of voices??
    for (size_t i = 0; i < positionArrays.size(); i++)
    {
        stream.WriteVector(positionArrays[i]);
        CHECK_THAT(stream.CheckState(), false);
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
    CHECK_THAT(stream.CheckState(), false);

    for (size_t i = 0; i < positionArrays.size(); i++)
    {
        stream.ReadVector(positionArrays[i], version);
        CHECK_THAT(stream.CheckState(), false);
    }

    return stream.CheckState();
}

/// Sets the clef used on the standard notation staff
/// @param clef Clef to set
/// @return True if the clef was set, false if not
bool Staff::SetClef(uint8_t clef)
{
    //------Last Checked------//
    // - Jan 5, 2005
    CHECK_THAT(clef <= 1, false);

    m_data &= ~clefMask;
    m_data |= (uint8_t)(clef << 4);

    return (true);
}

/// Sets the tablature staff type
/// @param type Tablature staff type to set
/// @return True if the tablature staff type was set, false if not
bool Staff::SetTablatureStaffType(uint8_t type)
{
    //------Last Checked------//
    // - Jan 5, 2005
    CHECK_THAT(Tuning::IsValidStringCount(type), false);

    m_data &= ~tablatureStaffTypeMask;
    m_data |= type;

    return (true);
}

// Calculates the height of the staff
int Staff::GetHeight() const
{
    return GetStandardNotationStaffAboveSpacing() + GetStandardNotationStaffBelowSpacing() + GetSymbolSpacing() +
            GetTablatureStaffBelowSpacing() + STD_NOTATION_LINE_SPACING * (STD_NOTATION_STAFF_TYPE - 1) +
            (GetTablatureStaffType() - 1) * 9 + 4 * STAFF_BORDER_SPACING; // TODO - pass in the tab line separation as a parameter
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

    if (!IsValidPositionIndex(voice, index))
        throw std::out_of_range("Invalid position index");

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

    for (auto i = positionArray.begin(); i != positionArray.end(); ++i)
    {
        if ((*i)->GetPosition() == index)
        {
            return *i;
        }
    }

    return NULL;
}

/// Finds the position index of the next position
/// @throw std::out_of_range if the position does not exist in this staff, or if the voice is invalid
size_t Staff::GetIndexOfNextPosition(uint32_t voice, System* system, Position *position) const
{
    if (!IsValidVoice(voice))
        throw std::out_of_range("Invalid voice");

    const std::vector<Position*>& positionArray = positionArrays.at(voice);

    auto location = std::find(positionArray.begin(), positionArray.end(), position);

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
bool Staff::IsOnlyPositionInBar(Position *position, System *system) const
{
    std::vector<Barline*> barlines;
    system->GetBarlines(barlines);

    Barline* prevBarline = system->GetPrecedingBarline(position->GetPosition());

    auto startBar = std::find(barlines.begin(), barlines.end(), prevBarline);
    auto endBar = startBar + 1;

    for (uint32_t i = (*startBar)->GetPosition() + 1; i < (*endBar)->GetPosition(); i++)
    {
        Position* pos = GetPositionByPosition(0, i); // TODO - support multiple voices
        if (pos != NULL && pos != position)
        {
            return false;
        }
    }

    return true;
}

bool Staff::CanHammerOn(Position* position, Note* note) const
{
    // In order to perform a hammer-on, the note must be lower than the next
    return CompareWithNote(NextNote, position, note, std::less<uint8_t>());
}

bool Staff::CanPullOff(Position* position, Note* note) const
{
    // In order to perform a pull-off, the note must be higher than the next
    return CompareWithNote(NextNote, position, note, std::greater<uint8_t>());
}

// Figures out if the given note can be set as tied
// The previous position in the staff must contain a Note at the same string & fret
bool Staff::CanTieNote(Position* position, Note* note) const
{
    // In order to tie, the note must be the same as the previous
    return CompareWithNote(PrevNote, position, note, std::equal_to<uint8_t>());
}

/// Determines if we can slide from the given note to the next note (shift or legato slides)
/// The next note must exist and be a different fret number
bool Staff::CanSlideBetweenNotes(Position *position, Note *note) const
{
    return CompareWithNote(NextNote, position, note, std::not_equal_to<uint8_t>());
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
        
    protected:
        T* object;
    };
}

/// Calculates the spacing required to display the given position and note properties.
int Staff::CalculateSpacingForProperties(const std::list<PositionProperty>& positionFunctions) const
{
    const std::vector<Position*>& positionArray = positionArrays[0]; // TODO - support multiple voices

    int maxNumProperties = 0;
    for (auto i = positionArray.begin(); i != positionArray.end(); ++i)
    {
        int numProperties = 0;
        
        // Check how many position properties are enabled at the current position
        TestPredicatePtr<Position> pred(*i);
        numProperties = std::count_if(positionFunctions.begin(), positionFunctions.end(), pred);
        
        // the highest number of properties enabled for a position in this system will determine the required height
        maxNumProperties = std::max(maxNumProperties, numProperties);
    }
    
    return maxNumProperties * TAB_SYMBOL_HEIGHT;
}

void Staff::CalculateTabStaffBelowSpacing()
{
    // Create list of all properties that are displayed below the tab staff
    std::list<PositionProperty> positionFunctions = {
        &Position::HasPickStrokeDown, &Position::HasPickStrokeUp, &Position::HasTap,
        &Position::HasNoteWithHammeronOrPulloff, &Position::HasNoteWithSlide,
        &Position::HasNoteWithTappedHarmonic, &Position::HasNoteWithArtificialHarmonic
    };

    SetTablatureStaffBelowSpacing(CalculateSpacingForProperties(positionFunctions));
}

void Staff::CalculateSymbolSpacing()
{
    // Create list of all properties that are displayed between the tab staff and std. notation staff
    std::list<PositionProperty> positionFunctions = {
        &Position::HasLetRing, &Position::HasVolumeSwell,
        &Position::HasVibrato, &Position::HasWideVibrato, &Position::HasPalmMuting,
        &Position::HasTremoloPicking, &Position::HasTremoloBar, &Position::HasNoteWithTrill,
        &Position::HasNoteWithNaturalHarmonic, &Position::HasNoteWithArtificialHarmonic
    };
    
    SetSymbolSpacing(CalculateSpacingForProperties(positionFunctions));
}

/// Calculates the beaming for notes that are located between the two given barlines
void Staff::CalculateBeamingForBar(const Barline* startBar, const Barline* endBar)
{
    // Get the positions in betwen the two bars
    std::vector<Position*> positions;
    GetPositionsInRange(positions, 0, startBar->GetPosition(), endBar->GetPosition());

    const TimeSignature& timeSig = startBar->GetTimeSignatureConstRef();

    // Get the beam group patterns from the time signature
    std::vector<uint8_t> beamGroupPatterns(4, 0);

    timeSig.GetBeamingPattern(beamGroupPatterns[0], beamGroupPatterns[1], beamGroupPatterns[2], beamGroupPatterns[3]);

    // Remove any beam group patterns of size 0 (not set)
    beamGroupPatterns.erase(std::remove(beamGroupPatterns.begin(), beamGroupPatterns.end(), 0),
                            beamGroupPatterns.end());

    // Create a list of the durations for each position
    std::vector<double> durations(positions.size());
    std::transform(positions.begin(), positions.end(),
                   durations.begin(), std::mem_fun(&Position::GetDuration));
    // Convert the duration list to a list of partial sums of the durations
    // (i.e. timestamps relative to the beginning of the bar)
    std::partial_sum(durations.begin(), durations.end(), durations.begin());

    double groupBeginTime = 0;
    auto pattern = beamGroupPatterns.begin();
    std::vector<double>::iterator groupStart = durations.begin(), groupEnd = durations.begin();

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

    auto beamableGroupStart = positions.begin();
    auto beamableGroupEnd = positions.begin();

    auto isBeamable = std::mem_fun(&Position::IsBeamable);

    // Clear all existing beaming information
    std::for_each(positions.begin(), positions.end(), std::mem_fun(&Position::ClearBeam));

    // find all subgroups of beamable notes (i.e. notes that aren't quarter notes, rests, etc)
    while (beamableGroupStart != positions.end())
    {
        // find the next range of consecutive positions that are beamable
        beamableGroupStart = std::find_if(beamableGroupEnd, positions.end(), isBeamable);
        beamableGroupEnd = std::find_if(beamableGroupStart, positions.end(), std::not1(isBeamable));

        for (auto i = beamableGroupStart; i != beamableGroupEnd; ++i)
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

/// Gets all of the positions within the given range (inclusive)
/// @param Output parameter, to store the positions that are in the range
void Staff::GetPositionsInRange(std::vector<Position*>& positionsInRange, uint32_t voice, size_t startPos, size_t endPos)
{
    if (!IsValidVoice(voice))
        throw std::out_of_range("Invalid voice");

    positionsInRange.clear();

    const std::vector<Position*>& positionArray = positionArrays.at(voice);

    for (size_t i = 0; i < positionArray.size(); i++)
    {
        Position* currentPosition = positionArray.at(i);
        const uint32_t location = currentPosition->GetPosition();

        if (location >= startPos && location <= endPos)
        {
            positionsInRange.push_back(currentPosition);
        }
    }
}

/// Returns the last position in the staff, regardless of voice (returns NULL if no positions exist)
Position* Staff::GetLastPosition() const
{
    using std::bind;
    using namespace std::placeholders;

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
                            bind(std::less<uint32_t>(),
                                 bind(&Position::GetPosition, _1),
                                 bind(&Position::GetPosition, _2)
                                 ));
}

/// Returns the number of steps (frets) between the given note and the next note on the string
/// @throws std::logic_error If there is no note at the same string for the next position
int8_t Staff::GetSlideSteps(Position *position, Note *note) const
{
    Note* nextNote = GetAdjacentNoteOnString(Staff::NextNote, position, note);

    if (!nextNote)
        throw std::logic_error("The next position does not have a note on the same string.");

    return nextNote->GetFretNumber() - note->GetFretNumber();
}

Note* Staff::GetAdjacentNoteOnString(SearchDirection searchDirection, Position *position, Note *note) const
{
    const std::vector<Position*>& positionArray = positionArrays[0]; // TODO - support multiple voices

    // find where the position is within the staff
    auto location = std::find(positionArray.begin(),
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

void Staff::UpdateTabNumber(Position *position, Note *note, uint8_t fretNumber)
{
    // hopefully this line will make it easier later when we handle multiple melodies
    const std::vector<Position*>& positionArray = positionArrays[0];

    // find the position in our vector of positions
    auto positionIt = std::find(positionArray.begin(), positionArray.end(), position);
    assert(positionIt != positionArray.end());

    note->SetFretNumber(fretNumber);

    if (positionIt != positionArray.begin())
    {
        // update note before this note
        Note *prevNote = (*(positionIt-1))->GetNoteByString(note->GetString());
        if (prevNote != NULL)
            UpdateNote((*(positionIt-1)), prevNote, note);
    }
    if (positionIt+1 != positionArray.end())
    {
        // update note after this note
        Note *nextNote = (*(positionIt+1))->GetNoteByString(note->GetString());
        if (nextNote != NULL)
            UpdateNote((*positionIt), note, nextNote);
    }
}

void Staff::UpdateNote(Position *prevPosition, Note *previousNote, Note *nextNote)
{
    const bool canPull = CanPullOff(prevPosition, previousNote);
    const bool canHammer = CanHammerOn(prevPosition, previousNote);

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
        if (!CanSlideBetweenNotes(prevPosition, previousNote))
        {
            previousNote->SetSlideOutOf(Note::slideOutOfNone, 0);
        }
        else
        {
            int8_t newSteps = GetSlideSteps(prevPosition, previousNote);
            previousNote->ClearSlideOutOf();
            previousNote->SetSlideOutOf(slideType, newSteps);
        }
    }

    if (nextNote->IsTied() && nextNote->GetFretNumber() != previousNote->GetFretNumber())
    {
        nextNote->SetTied(false);
    }
}
