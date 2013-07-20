// Copyright 2010 Edd Dawson.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef STD_FUNCTOR_SUPPORT_HPP_2138_07072010
#define STD_FUNCTOR_SUPPORT_HPP_2138_07072010

#include <boost/mpl/vector.hpp>
#include <boost/mpl/has_xxx.hpp>

namespace sigfwd
{
    namespace impl
    {
        BOOST_MPL_HAS_XXX_TRAIT_DEF(argument_type)
        BOOST_MPL_HAS_XXX_TRAIT_DEF(second_argument_type)

        template<typename StdFunctor>
        struct std_unary_functor_support
        {
            typedef boost::mpl::vector<const typename StdFunctor::argument_type &> params;
            static const int arity = 1;
        };

        template<typename StdFunctor>
        struct std_binary_functor_support
        {
            typedef boost::mpl::vector<const typename StdFunctor::first_argument_type &,
                                       const typename StdFunctor::second_argument_type &> params;
            static const int arity = 2;
        };

        struct unknown_functor
        {
            typedef void params;
        };

    } // close namespace impl

} // close namespace sigfwd

#endif // STD_FUNCTOR_SUPPORT_HPP_2138_07072010
