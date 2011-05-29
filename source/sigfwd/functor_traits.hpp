// Copyright 2010 Edd Dawson.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef FUNCTOR_TRAITS_HPP_1903_04072010
#define FUNCTOR_TRAITS_HPP_1903_04072010

#include <sigfwd/impl/std_functor_support.hpp>

#include <boost/type_traits/remove_pointer.hpp>
#include <boost/type_traits/remove_reference.hpp>

#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/function_arity.hpp>

#include <boost/mpl/if.hpp>

namespace sigfwd
{
    //! The primary template for the sigfwd::functor_traits class. It supports STL-style unary and binary function
    //! objects (those that expose, typedefs such as argument_type, first_argument_type, second_argument_type, ...).
    //!
    //! To support a particular type of functor, specialize functor_traits such that it has an internal typedef 
    //! "params" which should be a boost::mpl Sequence of the parameter types of the functor. It should also have a
    //! static integral constant "arity" whose value should be equal to the number of elements in "params".
    //!
    //! If it is simple to extract the call-signature from your functor, you can inherit from 
    //! functor_traits_from_call_sig in order to implement your specialization. For example, here's the specialization
    //! for boost::function provided by sigfwd:
    //! 
    //! \code
    //!  template<typename Sig>
    //!  struct functor_traits<boost::function<Sig> > : functor_traits_from_call_sig<Sig> { };
    //! \endcode
    template<typename Functor>
    struct functor_traits :
        boost::mpl::if_
        <
            impl::has_second_argument_type<Functor>,
            impl::std_binary_functor_support<Functor>,
            typename boost::mpl::if_
            <
                impl::has_argument_type<Functor>,
                impl::std_unary_functor_support<Functor>,
                impl::unknown_functor
            >
            ::type
        >
        ::type
    {
    };

    //! functor_traits_from_call_sig is useful for implementing specializations of functor_traits if you can extract
    //! a call signature from the type of a functor.
    //!
    //! \code
    //! functor_traits_from_call_sig<void (int, double)>::params // an mpl sequence containing int and double
    //! functor_traits_from_call_sig<float (float, bool, bool)>::arity // static integral constant with value 3
    //! \endcode
    template<typename CallSig>
    struct functor_traits_from_call_sig
    {
        typedef boost::function_types::parameter_types<CallSig> params;
        static const std::size_t arity = boost::function_types::function_arity<CallSig>::value;
    };

} // close namespace sigfwd

#endif // FUNCTOR_TRAITS_HPP_1903_04072010
