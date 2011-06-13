#include "edittimesignature.h"

#include <powertabdocument/score.h>
#include <powertabdocument/system.h>
#include <powertabdocument/barline.h>
#include <boost/foreach.hpp>

EditTimeSignature::EditTimeSignature(Score* score, const SystemLocation location,
                                     const TimeSignature& newTimeSig) :
    score(score),
    location(location),
    newTimeSig(newTimeSig)
{
    // save the original time signature
    Score::SystemConstPtr system = score->GetSystem(location.getSystemIndex());
    System::BarlineConstPtr barline = system->GetBarlineAtPosition(location.getPositionIndex());
    Q_ASSERT(barline);
    oldTimeSig = barline->GetTimeSignature();

    setText(QObject::tr("Edit Time Signature"));
}

void EditTimeSignature::redo()
{
    switchTimeSignatures(oldTimeSig, newTimeSig);
}

void EditTimeSignature::undo()
{
    switchTimeSignatures(newTimeSig, oldTimeSig);
}

/// Switches from the old time signature to the new time signature, starting at the position
/// stored in the "location" member
/// Modifies all following time signatures until a different time signature is reached
void EditTimeSignature::switchTimeSignatures(const TimeSignature& oldTime,
                                             const TimeSignature& newTime)
{
    const size_t startSystem = location.getSystemIndex();
    for (size_t i = startSystem; i < score->GetSystemCount(); i++)
    {
        std::vector<System::BarlinePtr> barlines;
        score->GetSystem(i)->GetBarlines(barlines);

        BOOST_FOREACH(System::BarlinePtr barline, barlines)
        {
            if (i == startSystem && barline->GetPosition() < location.getPositionIndex())
            {
                continue;
            }

            TimeSignature& time = barline->GetTimeSignatureRef();
            if (time.IsSameMeter(oldTime))
            {
                time.SetMeter(newTime.GetBeatsPerMeasure(), newTime.GetBeatAmount());

                if (i == startSystem && barline->GetPosition() == location.getPositionIndex())
                {
                    time.SetShown(newTime.IsShown());
                }
            }
            else
            {
                return;
            }
        }
    }
}
