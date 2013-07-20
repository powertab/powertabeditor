// Copyright 2010 Edd Dawson.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SIGNAL_FORWARDER_HPP_1911_04072010
#define SIGNAL_FORWARDER_HPP_1911_04072010

#include <boost/mpl/at.hpp>
#include <boost/noncopyable.hpp>
#include <boost/preprocessor/iteration/local.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/ref.hpp>
#include <boost/type_traits/remove_reference.hpp>

#include <QObject>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4180) // "qualifier applied to function type has no meaning; ignored"
#endif

namespace sigfwd
{
    struct connection;
    
    namespace impl
    {
        // Reimplements QObject's qt_metacall virtual function. qt_metacall is what gets called to invoke a slot when 
        // a connected signal is emitted.
        class signal_forwarder : public QObject, public boost::noncopyable
        {
            public:
                signal_forwarder(QObject *parent);
                virtual ~signal_forwarder();

                connection connect(QObject *emitter, const char *qt_signal_sig, const char *recv_sig, 
                                   Qt::ConnectionType conn_type, bool check_sigs);

                // An override of the method in the QObject base class. 
                // Determines whether the call is for us (or some other sub-object in the inheritance hierarchy) and if
                // so calls forward().
                virtual int qt_metacall(QMetaObject::Call call, int id, void **ret_and_argv);

            private:
                // To be implemented in a derived class where the arguments of argv are casted appropriately before
                // being passed to a function or functor of some kind.
                virtual void forward(void **argv) = 0;
        };

        template<typename T>
        void maybe_unused(const T&) { }

        template<typename Params, std::size_t N>
        struct nth_param : boost::remove_reference<typename boost::mpl::at_c<Params, N>::type> { };

        template<typename ParamTypes, std::size_t Arity>
        struct call;

#define CAST_NTH_ARGV_TO_ARG_TYPE(z, n, argv) \
    *static_cast<typename nth_param<ParamTypes, n>::type *>(argv[n])
  
#define BOOST_PP_LOCAL_MACRO(n) \
        template<typename ParamTypes> \
        struct call<ParamTypes, n> \
        { \
            template<typename Functor> \
            static void do_it(const Functor &f, void **argv) \
            { \
                maybe_unused(argv); \
                f(BOOST_PP_ENUM(n, CAST_NTH_ARGV_TO_ARG_TYPE, argv)); \
            } \
        };
  
// Make a number of partial specializations of call for different numbers of functor arities.
// A typical macro expansion will look like this:
//
//      template<typename ParamTypes>
//      struct call<ParamTypes, 3>
//      {
//          template<typename Functor>
//          static void do_it(const Functor &f, void **argv)
//          {
//              maybe_unused(argv);
//              f(
//                  *static_cast<typename nth_param<ParamTypes, 0>::type *>(argv[0]),
//                  *static_cast<typename nth_param<ParamTypes, 1>::type *>(argv[1]),
//                  *static_cast<typename nth_param<ParamTypes, 2>::type *>(argv[2])
//              );
//          }
//      };

#define BOOST_PP_LOCAL_LIMITS (0, 10)
#include BOOST_PP_LOCAL_ITERATE()

        // Implements the signal_forwarder interface.
        // Stores a copy of Functor which is invoked in the implementation of forward().
        template<typename Functor, typename FunctorTraits>
        class signal_forwarder_impl : public signal_forwarder
        {
            public:
                signal_forwarder_impl(QObject *parent, const Functor &receiver) :
                    signal_forwarder(parent),
                    receiver_(receiver)
                {
                }

            private:
                virtual void forward(void **argv)
                {
                    typedef typename boost::unwrap_reference<Functor>::type unwrapped_functor;

                    call
                    <
                        typename FunctorTraits::params,
                        FunctorTraits::arity
                    >
                    ::do_it(static_cast<unwrapped_functor &>(receiver_), argv);
                }

            private:
                Functor receiver_;
        };

    } // close namespace impl

} // close namespace sigfwd

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // SIGNAL_FORWARDER_HPP_1911_04072010
