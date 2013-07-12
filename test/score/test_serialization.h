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

#ifndef TEST_SCORE_SERIALIZATION_H
#define TEST_SCORE_SERIALIZATION_H

#include <catch.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/date_time/gregorian/formatters_limited.hpp>
#include <boost/date_time/gregorian/greg_serialize.hpp>
#include <boost/rational.hpp>
#include <boost/serialization/bitset.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/vector.hpp>
#include <sstream>

namespace boost {
    namespace serialization {

        /// Serialization for boost::rational. This should be moved into a
        /// common location once we support loading/saving the new file format.
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

        template <class Archive>
        inline void serialize(Archive &, boost::blank &, const unsigned int)
        {
        }

        template <>
        struct tracking_level< boost::blank >
            : mpl::int_<track_never> {} ;
    }
}

namespace Serialization {

    /// Basic test for the serialization code - we should be able to serialize
    /// and deserialize and object, and get an equivalent object back.
    template <typename T>
    void test(const T &original)
    {
        std::ostringstream output;
        boost::archive::text_oarchive oa(output);
        oa << original;

        T copy;
        std::istringstream input(output.str());
        boost::archive::text_iarchive ia(input);
        ia >> copy;

        REQUIRE(original == copy);
    }
}

#endif
