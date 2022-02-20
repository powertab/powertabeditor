/*
 * Copyright (C) 2022 Cameron White
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

#include "document.h"

#include <util/enumtostring.h>

using Ottavia = Gp7::Beat::Ottavia;
using Accidental = Gp7::ChordName::Note::Accidental;
using Alteration = Gp7::ChordName::Degree::Alteration;
using DirectionTarget = Gp7::MasterBar::DirectionTarget;
using DirectionJump = Gp7::MasterBar::DirectionJump;
using ClefType = Gp7::Bar::ClefType;
using HarmonicType = Gp7::Note::HarmonicType;
using FingerType = Gp7::Note::FingerType;

using namespace std::string_literals;

UTIL_DEFINE_ENUMTOSTRING(Ottavia, { { Ottavia::O8va, "8va"s },
                                    { Ottavia::O8vb, "8vb"s },
                                    { Ottavia::O15ma, "15ma"s },
                                    { Ottavia::O15mb, "15mb"s } })

UTIL_DEFINE_ENUMTOSTRING(Accidental,
                         { { Accidental::DoubleFlat, "DoubleFlat"s },
                           { Accidental::Flat, "Flat"s },
                           { Accidental::Natural, "Natural"s },
                           { Accidental::Sharp, "Sharp"s },
                           { Accidental::DoubleSharp, "DoubleSharp"s } })

UTIL_DEFINE_ENUMTOSTRING(Alteration,
                         { { Alteration::Perfect, "Perfect"s },
                           { Alteration::Diminished, "Diminished"s },
                           { Alteration::Augmented, "Augmented"s },
                           { Alteration::Major, "Major"s },
                           { Alteration::Minor, "Minor"s } })

UTIL_DEFINE_ENUMTOSTRING(DirectionTarget,
                         { { DirectionTarget::Fine, "Fine"s },
                           { DirectionTarget::Coda, "Coda"s },
                           { DirectionTarget::DoubleCoda, "DoubleCoda"s },
                           { DirectionTarget::Segno, "Segno"s },
                           { DirectionTarget::SegnoSegno, "SegnoSegno"s } })

UTIL_DEFINE_ENUMTOSTRING(
    DirectionJump,
    { { DirectionJump::DaCapo, "DaCapo"s },
      { DirectionJump::DaCapoAlCoda, "DaCapoAlCoda"s },
      { DirectionJump::DaCapoAlDoubleCoda, "DaCapoAlDoubleCoda"s },
      { DirectionJump::DaCapoAlFine, "DaCapoAlFine"s },
      { DirectionJump::DaSegno, "DaSegno"s },
      { DirectionJump::DaSegnoAlCoda, "DaSegnoAlCoda"s },
      { DirectionJump::DaSegnoAlDoubleCoda, "DaSegnoAlDoubleCoda"s },
      { DirectionJump::DaSegnoAlFine, "DaSegnoAlFine"s },
      { DirectionJump::DaSegnoSegno, "DaSegnoSegno"s },
      { DirectionJump::DaSegnoSegnoAlCoda, "DaSegnoSegnoAlCoda"s },
      { DirectionJump::DaSegnoSegnoAlDoubleCoda, "DaSegnoSegnoAlDoubleCoda"s },
      { DirectionJump::DaSegnoSegnoAlFine, "DaSegnoSegnoAlFine"s },
      { DirectionJump::DaCoda, "DaCoda"s },
      { DirectionJump::DaDoubleCoda, "DaDoubleCoda"s } })

UTIL_DEFINE_ENUMTOSTRING(ClefType, { { ClefType::G2, "G2" },
                                     { ClefType::F4, "F4" },
                                     { ClefType::C3, "C3" },
                                     { ClefType::C4, "C4" },
                                     { ClefType::Neutral, "Neutral" } })

UTIL_DEFINE_ENUMTOSTRING(HarmonicType,
                         { { HarmonicType::Natural, "Natural" },
                           { HarmonicType::Artificial, "Artificial" },
                           { HarmonicType::Pinch, "Pinch" },
                           { HarmonicType::Tap, "Tap" },
                           { HarmonicType::Semi, "Semi" },
                           { HarmonicType::Feedback, "Feedback" } })

UTIL_DEFINE_ENUMTOSTRING(FingerType, { { FingerType::Open, "Open" },
                                       { FingerType::C, "C" },
                                       { FingerType::A, "A" },
                                       { FingerType::M, "M" },
                                       { FingerType::I, "I" },
                                       { FingerType::P, "P" } })
