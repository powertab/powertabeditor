// Copyright 2010 Edd Dawson.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef CONNECT_HELPERS_HPP_2114_07072010
#define CONNECT_HELPERS_HPP_2114_07072010

#include <sigfwd/connection.hpp>

#include <boost/function_types/function_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/push_front.hpp>

#include <QObject>

namespace sigfwd
{
    // Forward declaration
    template<typename CallSig, typename Functor>
    connection connectx(QObject *emitter, const char *qt_signal_sig, const Functor &receiver, 
                        Qt::ConnectionType conn_type, bool check_sigs);

    namespace impl
    {
        // An implementation detail of sigfwd::connect().
        //
        // Assuming UnwrappedFunctor is a function object class with a single and non-templated function call operator,
        // &UnwrappedFunctor::operator() is passed to a template function (do_it_impl) wherein the type deduced by the
        // template function instantiation can be used as the call signature for sigfwd::connectx.
        //
        // This technique was suggested by Daniel Walker on the boost developers mailing list.
        template<typename UnwrappedFunctor, typename Functor>
        struct call_connectx_with_sig_from_call_operator
        {
            template<typename MethodSig>
            static connection do_it_impl(MethodSig, QObject *emitter, const char *qt_signal_sig, 
                                         const Functor &receiver, 
                                         Qt::ConnectionType conn_type, bool check_sigs)
            {
                typedef boost::function_types::parameter_types<MethodSig> full_params;

                // Remove the UnwrappedFunctor "argument" from the method call parameters...
                typedef typename boost::mpl::pop_front<full_params>::type params; 

                // ... add a return type (doesn't matter what, void is fine) ...
                typedef typename boost::mpl::push_front<params, void>::type ret_and_params;

                // ... and we can now synthesize a call signature for UnwrappedFunctor:
                typedef typename boost::function_types::function_type<ret_and_params>::type sig;

                return connectx<sig>(emitter, qt_signal_sig, receiver, conn_type, check_sigs);
            }

            static connection do_it(QObject *emitter, const char *qt_signal_sig, 
                                    Functor receiver, 
                                    Qt::ConnectionType conn_type, bool check_sigs)
            {
                return do_it_impl(&UnwrappedFunctor::operator(), 
                                  emitter, qt_signal_sig, 
                                  receiver, 
                                  conn_type, check_sigs);
            }
        };

        // An implementation detail of sigfwd::connect().
        //
        // This is called when we've found a specialization of sigfwd::functor_traits<> for Functor. In this case,
        // we can synthesize a call signature from FunctorTraits::params.
        template<typename FunctorTraits, typename Functor>
        struct call_connectx_with_sig_from_traits
        {
            static connection do_it(QObject *emitter, const char *qt_signal_sig, Functor receiver, 
                                    Qt::ConnectionType conn_type, bool check_sigs)
            {
                // Add a return type to the params list (anthing will do, void is fine)...
                typedef typename boost::mpl::push_front<typename FunctorTraits::params, void>::type ret_and_params;

                // ... and we can now synthesize a call signature for Functor:
                typedef typename boost::function_types::function_type<ret_and_params>::type sig;

                return connectx<sig>(emitter, qt_signal_sig, receiver, conn_type, check_sigs);
            }
        };

    } // close namespace impl

} // close namespace sigfwd

#endif // CONNECT_HELPERS_HPP_2114_07072010
