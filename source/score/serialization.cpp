/*
  * Copyright (C) 2013 Cameron White
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

#include "serialization.h"

#include <boost/date_time/gregorian/formatters_limited.hpp>
#include <boost/date_time/gregorian/greg_serialize.hpp>
#include <boost/rational.hpp>
#include <boost/serialization/bitset.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/vector.hpp>

#include "score.h"
#include "staff.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
typedef boost::archive::text_iarchive InputArchive;
typedef boost::archive::text_oarchive OutputArchive;

namespace boost {
    namespace serialization {

        /// Implement serialization for boost::rational.
        template <class Archive, class I>
        inline void serialize(Archive &ar, boost::rational<I> &num, const unsigned int version)
        {
            boost::serialization::split_free(ar, num, version);
        }

        template <class Archive, class I>
        void save(Archive &ar, const boost::rational<I> &num, const unsigned int /*version*/)
        {
            I numerator = num.numerator();
            I denominator = num.denominator();
            ar << numerator << denominator;
        }

        template <class Archive, class I>
        void load(Archive &ar, boost::rational<I> &num, const unsigned int /*version*/)
        {
            I numerator;
            I denominator;
            ar >> numerator >> denominator;
            num.assign(numerator, denominator);
        }

        template <class I>
            struct is_bitwise_serializable< boost::rational<I> >
            : public is_bitwise_serializable< I > {};

        template <class I>
            struct implementation_level< boost::rational<I> >
            : mpl::int_<object_serializable> {} ;

        template <class I>
            struct tracking_level< boost::rational<I> >
            : mpl::int_<track_never> {} ;

        /// Implement serialization for boost::blank.
        template <class Archive>
        inline void serialize(Archive &, boost::blank &, const unsigned int)
        {
        }

        template <>
        struct tracking_level< boost::blank >
            : mpl::int_<track_never> {} ;
    }
}

namespace ScoreUtils {

#define SERIALIZE(T) \
    template <> \
    void load<T>(std::istream &input, T &obj) \
    { \
        InputArchive ia(input); \
        ia >> obj; \
    } \
    template <> \
    void save<T>(std::ostream &output, const T &obj) \
    { \
        OutputArchive oa(output); \
        oa << obj; \
    }

// Provide explicit instantiations for all of the possible types that
// can be serialized.
SERIALIZE(AlternateEnding)
SERIALIZE(Barline)
SERIALIZE(ChordName)
SERIALIZE(Direction)
SERIALIZE(Dynamic)
SERIALIZE(Instrument)
SERIALIZE(KeySignature)
SERIALIZE(Note)
SERIALIZE(Player)
SERIALIZE(PlayerChange)
SERIALIZE(Position)
SERIALIZE(RehearsalSign)
SERIALIZE(Score)
SERIALIZE(ScoreInfo)
SERIALIZE(Staff)
SERIALIZE(System)
SERIALIZE(TempoMarker)
SERIALIZE(TimeSignature)
SERIALIZE(Tuning)

// Used for the clipboard functions.
SERIALIZE(int)
SERIALIZE(std::vector<Position>)

// Used for the tuning dictionary.
SERIALIZE(std::vector<Tuning>)

}
