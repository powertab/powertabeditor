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
  
#include "harmonics.h"

#include <map>

namespace Harmonics
{
    namespace
    {
        std::map<uint8_t, uint8_t> harmonicsMap; ///< maps fret offsets to pitch offsets

        /// initialize the mapping of frets to pitch offsets (counted in half-steps or frets)
        /// e.g. The natural harmonic at the 7th fret is an octave and a fifth - 19 frets - above the pitch of the open string
        void initHarmonicsMap()
        {
            harmonicsMap[3] = 31;
            harmonicsMap[4] = harmonicsMap[9] = 28;
            harmonicsMap[16] = harmonicsMap[28] = 28;
            harmonicsMap[5] = harmonicsMap[24] = 24;
            harmonicsMap[7] = harmonicsMap[19] = 19;
            harmonicsMap[12] = 12;
        }
    }

    /// Returns the pitch offset corresponding to the given fret offset
    /// e.g. a fret offset of 12 has a pitch offset of 12 (one octave), and
    /// a fret offset of 7 has pitch offset 19 (octave and a fifth)
    uint8_t getPitchOffset(uint8_t fretOffset)
    {
        initHarmonicsMap();

        std::map<uint8_t, uint8_t>::const_iterator pitchOffset = harmonicsMap.find(fretOffset);
        return (pitchOffset == harmonicsMap.end()) ? 0 : pitchOffset->second;
    }

    /// Returns a list of all fret offsets that produce harmonics
    std::vector<uint8_t> getFretOffsets()
    {
        initHarmonicsMap();

        std::vector<uint8_t> frets;
        for (std::map<uint8_t, uint8_t>::const_iterator i = harmonicsMap.begin();
             i != harmonicsMap.end(); ++i)
        {
            frets.push_back(i->first);
        }

        return frets;
    }
}
