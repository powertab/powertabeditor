// Copyright 2010 Edd Dawson.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "sigfwd/impl/sig_string.hpp"

#ifdef __GNUC__
#include <cxxabi.h>
#include <cstdlib>
#include <new>
#endif

#include <map>

namespace sigfwd
{
    namespace impl
    {
        namespace
        {
            struct type_data
            {
                type_data(const std::type_info &info) : info_(&info) { }
                const std::type_info *info_;
            };

            bool operator< (const type_data &lhs, const type_data &rhs) 
            {
                return lhs.info_->before(*rhs.info_);
            }

            std::map<type_data, std::string> g_type_2_name;
        }

        void register_type_name(const std::type_info &info, const std::string &name)
        {
            g_type_2_name[info] = name;
        }

        std::string std_rtti_type_name(const std::type_info &info)
        {
#ifdef __GNUC__
            int status = 0;
            char *d = 0;
            std::string ret;

            try 
            {
                if ((d = abi::__cxa_demangle(info.name(), 0, 0, &status)))
                    ret = d;
            }
            catch (const std::bad_alloc &)
            {
                std::free(d);
                throw;
            }

            std::free(d);
            return ret;
#else
            return info.name();
#endif
        }

        std::string type_name(const std::type_info &info)
        {
            std::map<type_data, std::string>::const_iterator f = g_type_2_name.find(info);
            return f != g_type_2_name.end() ? f->second : std_rtti_type_name(info); 
        }
    }
}
