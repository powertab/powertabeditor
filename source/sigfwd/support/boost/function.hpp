// Copyright 2010 Edd Dawson.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_FUNCTION_HPP_2332_05072010
#define BOOST_FUNCTION_HPP_2332_05072010

#include <sigfwd/functor_traits.hpp>

#include <boost/function.hpp>

#include <boost/preprocessor/iteration/local.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/comma_if.hpp>
#include <boost/preprocessor/cat.hpp>

#include <boost/version.hpp>

namespace sigfwd
{

#if BOOST_VERSION < 103600
    // Versions of boost::function prior to 1.36 had a allocator template argument.

    // Specialization for boost::function
    template<typename Sig, typename A>
    struct functor_traits<boost::function<Sig, A> > :
        functor_traits_from_call_sig<Sig> 
    {
    };

    // Specializations for boost::function0, boost::function1, boost::function2, ...
#define BOOST_PP_LOCAL_MACRO(n) \
    template<typename R BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, typename T), typename A> \
    struct functor_traits<boost:: BOOST_PP_CAT(function, n) <R BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, T), A> > : \
        functor_traits_from_call_sig<R (BOOST_PP_ENUM_PARAMS(n, T))> \
    { \
    };

#else

    // Specialization for boost::function
    template<typename Sig>
    struct functor_traits<boost::function<Sig> > :
        functor_traits_from_call_sig<Sig> 
    {
    };

    // Specializations for boost::function0, boost::function1, boost::function2, ...
#define BOOST_PP_LOCAL_MACRO(n) \
    template<typename R BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, typename T)> \
    struct functor_traits<boost:: BOOST_PP_CAT(function, n) <R BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, T)> > : \
        functor_traits_from_call_sig<R (BOOST_PP_ENUM_PARAMS(n, T))> \
    { \
    };

#endif


#define BOOST_PP_LOCAL_LIMITS (0, 10) // TODO: see if there's a macro for 10
#include BOOST_PP_LOCAL_ITERATE()


} // close namespace sigfwd

#endif // BOOST_FUNCTION_HPP_2332_05072010
