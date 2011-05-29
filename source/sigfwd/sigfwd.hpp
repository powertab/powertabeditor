// Copyright 2010 Edd Dawson.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SIGFWD_HPP_2341_05072010
#define SIGFWD_HPP_2341_05072010

// This header includes everything sigfwd has to offer!

#include <boost/version.hpp>
#include <sigfwd/support/boost/function.hpp>
/*#include <sigfwd/support/boost/signals.hpp>
#if BOOST_VERSION >= 103900
#include <sigfwd/support/boost/signals2.hpp>
#endif*/
#include <sigfwd/support/boost/bind.hpp>

// I think it's pretty much impossible to support boost::lambda, unfortunately.

#include <sigfwd/connect.hpp>
#include <sigfwd/type_registry.hpp>
#include <sigfwd/version.hpp>

#endif // SIGFWD_HPP_2341_05072010
