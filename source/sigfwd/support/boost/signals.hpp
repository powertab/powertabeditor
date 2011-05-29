// Copyright 2010 Edd Dawson.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SIGNALS_HPP_2334_05072010
#define BOOST_SIGNALS_HPP_2334_05072010

#if defined(signals) // may be defined by Qt, can't include <boost/signals.hpp> in this case

    #if !defined(SIGFWD_NO_BOOST_SIGNALS_MESSAGE)
        #if defined(_MSC_VER)
            #pragma message("sigfwd can't support the boost::signals library when 'signals' is defined.")
            #pragma message("Define SIGFWD_NO_BOOST_SIGNALS_MESSAGE to remove this warning.")
        #elif defined(__GNUC__)
            #warning "sigfwd can't support the boost::signals library when 'signals' is defined."
            #warning "Define SIGFWD_NO_BOOST_SIGNALS_MESSAGE to remove this warning."
        #endif
    #endif

#else

#include <sigfwd/functor_traits.hpp>
#include <boost/signals.hpp>

namespace sigfwd
{
    // Specialization for boost::signal
    template<typename Sig, typename C, typename G, typename S, typename E>
    struct functor_traits<boost::signal<Sig, C, G, S, E> > :
        functor_traits_from_call_sig<Sig> 
    {
    };

} // close namespace sigfwd

#endif // defined(signals)

#endif // BOOST_SIGNALS_HPP_2334_05072010
