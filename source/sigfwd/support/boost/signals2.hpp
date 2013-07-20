// Copyright 2010 Edd Dawson.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SIGNALS2_HPP_2337_05072010
#define BOOST_SIGNALS2_HPP_2337_05072010

#include <sigfwd/functor_traits.hpp>
#include <boost/signals2/signal.hpp>

namespace sigfwd
{
    // Specialization for boost::signals2::signal
    template<typename Sig, typename C, typename G, typename S, typename E, typename M>
    struct functor_traits<boost::signals2::signal<Sig, C, G, S, E, M> > : 
        functor_traits_from_call_sig<Sig> 
    {
    };

} // close namespace sigfwd

#endif // BOOST_SIGNALS2_HPP_2337_05072010
