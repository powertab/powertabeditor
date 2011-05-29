// Copyright 2010 Edd Dawson.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef CONNECT_HPP_1704_04072010
#define CONNECT_HPP_1704_04072010

#include <sigfwd/connection.hpp>
#include <sigfwd/impl/connect_helpers.hpp>
#include <sigfwd/impl/sig_string.hpp>
#include <sigfwd/impl/signal_forwarder.hpp>

#include <boost/mpl/if.hpp>
#include <boost/ref.hpp>
#include <boost/type_traits/add_pointer.hpp>
#include <boost/type_traits/is_function.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/type_traits/remove_reference.hpp>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4180) // "qualifier applied to function type has no meaning; ignored"
#endif

namespace sigfwd
{
    //! Connects the signal from the given \a emitter identified by \a qt_signal_sig to the specified \a receiver.
    //! \a receiver is a function or other callable object with the call signature given by \a CallSig. 
    //! \a qt_signal_sig should be the result of an application of Qt's SIGNAL macro. The \a conn_type specifies the 
    //! kind of connection to create.
    //!
    //! \code
    //! // assuming my_functor has an operator() that can be called with an int argument
    //! connectx<void (int)>(emitter, SIGNAL(valueChanged(int)), my_functor);
    //! \endcode
    //!
    //! The function returns a connection with a "connected" status if the connection was made successfully. On failure,
    //! the qobj member of the returned connection will be null valued.
    //!
    //! In most cases, it's easier to use sigfwd::connect, rather than this function. Use this function when you
    //! need to connect a signal to a slot whose signature cannot be analysed.
    //! 
    //! If \a check_sigs is true, sigfwd will attempt to check that \a CallSig is compatible with the call signature
    //! of the specified Qt signal. In order to do this, a string representation for CallSig is created using C++'s
    //! RTTI machinery (specifically sigfwd tries to demangle std::type_info::name()). 
    //!
    //! Sometimes this won't work as demangle(typeid(std::string).name()) might result in 
    //! "std::basic_string<char, std::char_traits<char>, std::allocator<char>" whereas the corresponding argument in
    //! \a qt_signal_sig might be spelled "std::string". In situations like these, you can pass false for \a check_sigs,
    //! but be warned that you are on your own as far as checking signature compatability is concerned. Connections made
    //! for incompatible signals/slows will result in undefined behaviour.
    //!
    //! Internally a copy of \a receiver is made. If you want to use an un-copyable type of functor, or wish to avoid a
    //! copy being made, you can wrap a function in a boost::reference_wrapper usig boost::ref or boost::cref.
    template<typename CallSig, typename Functor>
    connection connectx(QObject *emitter, const char *qt_signal_sig, const Functor &receiver, 
                        Qt::ConnectionType conn_type = Qt::AutoConnection, bool check_sigs = true)
    {
        using namespace boost;

        // support passing functions (not just pointers to functions) for receiver.
        typedef typename mpl::if_<is_function<Functor>, typename add_pointer<Functor>::type, Functor>::type Functor_;

        typedef functor_traits_from_call_sig<CallSig> traits;
        const std::string recv_sig = impl::sig_string<typename traits::params>();

        // The signal_forwarder that we create below will be a QObject owned by emitter.
        // So despite appearances, we aren't really leaking an object here.
        impl::signal_forwarder *fwd = new impl::signal_forwarder_impl<Functor_, traits>(emitter, receiver);
        connection con = fwd->connect(emitter, qt_signal_sig, recv_sig.c_str(), conn_type, check_sigs);

        if (!con)
        {
            delete con.qobj;
            con.qobj = 0;
        }

        return con;
    }
    
    //! Attempts to deduce the call signature of Functor. Assuming it is successfully deduced as Sig, this
    //! function is equivalent to:
    //!
    //! \code
    //! connectx<Sig>(emitter, qt_signal_sig, receiver, conn_type, check_sigs).
    //! \endcode
    //!
    //! connect can deduce Sig in the following cases:
    //!
    //! 1. Functor is a function, function pointer or function reference
    //! 2. a specialization of sigfwd::functor_traits<> for Functor is in scope
    //! 3. Functor is an STL-style function object that is callable with one or two arguments i.e. it exposes typedefs
    //!    such as first_argument_type or first_argument_type and second_argument_type
    //! 4. Functor has a single, non-template operator() overload
    //!
    //! Many specializations of sigfwd::functor_traits<> are supplied with sigfwd for widely used functors. See the 
    //! headers under sigfwd/support.
    //!
    //! If Sig cannot be deduced from Functor, you will have to use sigfwd::connectx instead and specify Functor's call
    //! signature manually.
    //!
    //! See connectx for more information on the meaning of the arguments and possible return values.
    template<typename Functor>
    connection connect(QObject *emitter, const char *qt_signal_sig, const Functor &receiver, 
                       Qt::ConnectionType conn_type = Qt::AutoConnection, bool check_sigs = true)
    {
        using namespace boost;
        using namespace boost::mpl;

        // Users can pass boost::reference_wrapper<>s for receiver if copies need to be prevented.
        // We'll need to remove any such dressing in order to deduce the call signature.
        typedef typename boost::unwrap_reference<Functor>::type unwrapped_functor;

        // Remove any pointer or reference modifiers. In most cases this will do nothing, but this helps
        // us to detect support function pointers and references to functions.
        typedef typename remove_reference<typename remove_pointer<unwrapped_functor>::type>::type sig;
        typedef functor_traits<unwrapped_functor> traits;

        // Here's the pseduo-code of the compile-time stuff below:
        //
        //  if sig is a function:
        //      call connectx<sig>(...)
        //  else if sigfwd::functor_traits can't find a call signature
        //      call connectx<OpSig>(...) where OpSig is derived from the type of &unwrapped_functor::operator()
        //  else
        //      call connectx<TraitsSig>(...) where TraitsSig is synthesized from unwrapped_functor's functor_traits.

        return if_
        <
            is_function<sig>,
            impl::call_connectx_with_sig_from_traits<functor_traits_from_call_sig<sig>, Functor>,
            typename if_
            <
                is_same<void, typename traits::params>, // i.e. is functor_traits un-specialized?
                impl::call_connectx_with_sig_from_call_operator<unwrapped_functor, Functor>,
                impl::call_connectx_with_sig_from_traits<traits, const Functor &>
            >
            ::type
        >
        ::type::do_it(emitter, qt_signal_sig, receiver, conn_type, check_sigs);
    }

} // close namespace sigfwd

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // CONNECT_HPP_1704_04072010
