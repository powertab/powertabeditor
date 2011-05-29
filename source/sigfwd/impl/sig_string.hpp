// Copyright 2010 Edd Dawson.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SIG_STRING_HPP_1905_04072010
#define SIG_STRING_HPP_1905_04072010

#include <sigfwd/functor_traits.hpp>

#include <boost/mpl/at.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/size.hpp>

#include <cassert>
#include <cstddef>
#include <string>
#include <typeinfo>

#include <QMetaType>

namespace sigfwd
{
    namespace impl
    {
        // Register the name of a type. Registered names are used by sigfwd in order to compose text representations
        // of functor call signatures. If a type is not registered, the name will be sought from RTTI, using 
        // std_rtti_type_name().
        // NOT THREAD SAFE. Do all your regsitration up-front, if possible.
        void register_type_name(const std::type_info &info, const std::string &name);

        // This function attempts to return the demangled name of the C++ type corresponding to info.
        std::string std_rtti_type_name(const std::type_info &info);

        // First looks for a name registered with register_type_name. Failing that uses std_rtti_type_name.
        std::string type_name(const std::type_info &info);

        template<typename T>
        struct get_name_from_qt_meta_system
        {
            static const char *do_it()
            {
                const char *name = QMetaType::typeName(qMetaTypeId<T>());
                assert(name);
                return name;
            }
        };

        template<typename T>
        struct get_name_from_elsewhere
        {
            static std::string do_it() { return type_name(typeid(T)); }
        };

        //! Returns the name of the given type.
        //! If the type was registered with Qt's meta system using Q_DECLARE_METATYPE, the name is sought from
        //! qMetaTypeId, else type_name(typeid(T)) is used.
        //! The intention is to return a string that matches Qt meta-system's notion of T's name as best as possible.
        template<typename T>
        std::string meta_type_name()
        {
            return boost::mpl::if_c
            <
                QMetaTypeId<T>::Defined,
                get_name_from_qt_meta_system<T>,
                get_name_from_elsewhere<T>
            >
            ::type::do_it();
        }

        template<typename ParamTypes, std::size_t ParamIndex, std::size_t Arity>
        struct arg_string
        {
            static std::string do_it()
            {
                typedef typename boost::mpl::at_c<ParamTypes, ParamIndex>::type param;
                return (ParamIndex == 0 ? "" : ", ") + 
                       meta_type_name<param>() +
                       arg_string<ParamTypes, ParamIndex + 1, Arity>::do_it();
            }
        };

        template<typename ParamTypes, std::size_t Arity>
        struct arg_string<ParamTypes, Arity, Arity>
        {
            static std::string do_it() { return ""; }
        };

        // Given an mpl sequence of types, \a Params, this returns a string of the form:
        // "f(" + typename(typeid(Params[0])) + ", " + ... + ", " + typeid(Params[n-1]) + ")"
        // i.e. it returns something that you might find inside a Qt SLOT macro invocation.
        template<typename Params>
        std::string sig_string()
        {
            return "f(" + arg_string<Params, 0, boost::mpl::size<Params>::value>::do_it() + ')';
        }

    } // close namespace sigfwd

} // close namespace sigfwd

#endif // SIG_STRING_HPP_1905_04072010
