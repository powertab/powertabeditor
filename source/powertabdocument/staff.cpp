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

#include "system.h"
#include "powertabfileheader.h"             // Needed for file version constants
#include "tuning.h"                         // Needed for IsValidStringCount

#include <numeric> // partial_sum

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
    //------Last Checked------//
    // - Jan 5, 2005
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
    //------Last Checked------//
    // - Jan 5, 2005
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
    //------Last Checked------//
    // - Jan 5, 2005
    *this = staff;
}

/// Destructor
Staff::~Staff()
{
    //------Last Checked------//
    // - Jan 5, 2005
    for (uint32_t i = 0; i < highMelodyPositionArray.size(); i++)
    {
        delete highMelodyPositionArray.at(i);
    }
    for (uint32_t i = 0; i < lowMelodyPositionArray.size(); i++)
    {
        delete lowMelodyPositionArray.at(i);
    }
    highMelodyPositionArray.clear();
    lowMelodyPositionArray.clear();
}

/// Assignment Operator
const Staff& Staff::operator=(const Staff& staff)
{
    //------Last Checked------//
    // - Jan 5, 2005

    // Check for assignment to self
    if (this != &staff)
    {
        m_data = staff.m_data;
        m_standardNotationStaffAboveSpacing =
            staff.m_standardNotationStaffAboveSpacing;
        m_standardNotationStaffBelowSpacing =
            staff.m_standardNotationStaffBelowSpacing;
        m_symbolSpacing = staff.m_symbolSpacing;
        m_tablatureStaffBelowSpacing = staff.m_tablatureStaffBelowSpacing;

        highMelodyPositionArray = staff.highMelodyPositionArray;
        lowMelodyPositionArray = staff.lowMelodyPositionArray;
    }
    return (*this);
}

Staff* Staff::CloneObject() const
{
    Staff* newStaff = new Staff;

    newStaff->m_data = m_data;
    newStaff->m_standardNotationStaffAboveSpacing = m_standardNotationStaffAboveSpacing;
    newStaff->m_standardNotationStaffBelowSpacing = m_standardNotationStaffBelowSpacing;
    newStaff->m_symbolSpacing = m_symbolSpacing;
    newStaff->m_tablatureStaffBelowSpacing = m_tablatureStaffBelowSpacing;

    for (uint32_t i=0; i < highMelodyPositionArray.size(); i++)
    {
        newStaff->highMelodyPositionArray.push_back(highMelodyPositionArray.at(i)->CloneObject());
    }
    for (uint32_t i=0; i < lowMelodyPositionArray.size(); i++)
    {
        newStaff->lowMelodyPositionArray.push_back(lowMelodyPositionArray.at(i)->CloneObject());
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
        (highMelodyPositionArray == staff.highMelodyPositionArray) &&
        (lowMelodyPositionArray == staff.lowMelodyPositionArray)
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

    stream.WriteVector(highMelodyPositionArray);
    CHECK_THAT(stream.CheckState(), false);

    stream.WriteVector(lowMelodyPositionArray);
    CHECK_THAT(stream.CheckState(), false);

    return (stream.CheckState());
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool Staff::Deserialize(PowerTabInputStream& stream, uint16_t version)
{
    //------Last Checked------//
    // - Jan 5, 2005

    // Version 1.0 and 1.0.2, music/tab staff type stored in separate variables
    if (version == PowerTabFileHeader::FILEVERSION_1_0 ||
        version == PowerTabFileHeader::FILEVERSION_1_0_2)
    {
        uint8_t clef, tablatureStaffType;
        stream >> clef >> tablatureStaffType >>
            m_standardNotationStaffAboveSpacing >>
            m_standardNotationStaffBelowSpacing >> m_symbolSpacing >>
            m_tablatureStaffBelowSpacing;
        CHECK_THAT(stream.CheckState(), false);

        SetClef(clef);
        SetTablatureStaffType(tablatureStaffType);

        stream.ReadVector(highMelodyPositionArray, version);
        CHECK_THAT(stream.CheckState(), false);

        stream.ReadVector(lowMelodyPositionArray, version);
        CHECK_THAT(stream.CheckState(), false);
    }
    // Version 1.5 and up
    else
    {
        stream >> m_data >> m_standardNotationStaffAboveSpacing >>
            m_standardNotationStaffBelowSpacing >> m_symbolSpacing >>
            m_tablatureStaffBelowSpacing;
        CHECK_THAT(stream.CheckState(), false);

        stream.ReadVector(highMelodyPositionArray, version);
        CHECK_THAT(stream.CheckState(), false);

        stream.ReadVector(lowMelodyPositionArray, version);
        CHECK_THAT(stream.CheckState(), false);
    }

    return (stream.CheckState());
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

// Finds the Position object at the given position index in the staff
Position* Staff::GetPositionByPosition(uint32_t index) const
{
    for (uint32_t i = 0; i < highMelodyPositionArray.size(); i++)
    {
        if (highMelodyPositionArray.at(i)->GetPosition() == index)
        {
            return highMelodyPositionArray.at(i);
        }
    }

    return NULL;
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
        Position* pos = GetPositionByPosition(i);
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
    int maxNumProperties = 0;
    for (auto i = highMelodyPositionArray.begin(); i != highMelodyPositionArray.end(); ++i)
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
    GetPositionsInRange(positions, startBar->GetPosition(), endBar->GetPosition());

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
void Staff::GetPositionsInRange(std::vector<Position*>& positions, size_t startPos, size_t endPos)
{
    positions.clear();

    for (size_t i = 0; i < highMelodyPositionArray.size(); i++)
    {
        Position* currentPosition = highMelodyPositionArray.at(i);
        const uint32_t location = currentPosition->GetPosition();

        if (location >= startPos && location <= endPos)
        {
            positions.push_back(currentPosition);
        }
    }
}

/// Returns the last position in the staff (returns NULL if no positions exist)
Position* Staff::GetLastPosition() const
{
    if (highMelodyPositionArray.empty())
        return NULL;

    return highMelodyPositionArray.back();
}
