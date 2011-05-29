// Copyright 2010 Edd Dawson.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef TYPE_REGISTRY_HPP_0229_08072010
#define TYPE_REGISTRY_HPP_0229_08072010

#include <sigfwd/impl/sig_string.hpp>

namespace sigfwd
{
    //! Tells sigfwd that the textual representation of the type T is given by \a name. This is useful for types 
    //! such as std::string, which might otherwise be known as something like 
    //! std::basic_string<char, std::char_traits<char>, std::allocator<char> > in the C++ RTTI machinery.
    template<typename T>
    void register_type_name(const std::string &name)
    {
        impl::register_type_name(typeid(T), name);
    }

    //! Calls regsiter_type_name<T>(name) and qRegisterMetaType<T>(name).
    template<typename T>
    void register_meta_type(const std::string &name)
    {
        impl::register_type_name(typeid(T), name);
        qRegisterMetaType<T>(name.c_str());
    }

    //! Calls regsiter_type_name<T>(name), qRegisterMetaType<T>(name) and qRegisterMetaTypeStreamOperators<T>(name).
    template<typename T>
    void register_streamable_meta_type(const std::string &name)
    {
        impl::register_type_name(typeid(T), name);
        qRegisterMetaType<T>(name.c_str());
        qRegisterMetaTypeStreamOperators<T>(name.c_str());
    }

} // close namespace sigfwd

#endif // TYPE_REGISTRY_HPP_0229_08072010
