#include "editkeysignature.h"

#include <powertabdocument/score.h>
#include <powertabdocument/system.h>
#include <powertabdocument/barline.h>
#include <boost/foreach.hpp>

EditKeySignature::EditKeySignature(Score* score, const SystemLocation& location,
                                   uint8_t newKeyType, uint8_t newKeyAccidentals, bool isShown) :
    score(score),
    location(location),
    newKeySig(newKeyType, newKeyAccidentals)
{
    newKeySig.SetShown(isShown);
    // save the original key signature
    Score::SystemConstPtr system = score->GetSystem(location.getSystemIndex());
    System::BarlineConstPtr barline = system->GetBarlineAtPosition(location.getPositionIndex());
    Q_ASSERT(barline);
    oldKeySig = barline->GetKeySignature();

    setText(QObject::tr("Edit Key Signature"));
}

void EditKeySignature::redo()
{
    switchKeySignatures(oldKeySig, newKeySig);
    emit triggered();
}

void EditKeySignature::undo()
{
    switchKeySignatures(newKeySig, oldKeySig);
    emit triggered();
}

/// Switches from the old key signature to the new key signature, starting at the position
/// stored in the "location" member
/// Modifies all following key signatures until a different key signature is reached
void EditKeySignature::switchKeySignatures(const KeySignature& oldKey,
                                           const KeySignature& newKey)
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

            KeySignature& key = barline->GetKeySignatureRef();
            if (key.IsSameKey(oldKey))
            {
                key.SetKey(newKey.GetKeyType(), newKey.GetKeyAccidentals());

                if (i == startSystem && barline->GetPosition() == location.getPositionIndex())
                {
                    key.SetShown(newKey.IsShown());
                }
            }
            else
            {
                return;
            }
        }
    }
}
