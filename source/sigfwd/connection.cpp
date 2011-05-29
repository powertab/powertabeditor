// Copyright 2010 Edd Dawson.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "sigfwd/connection.hpp"

#include <QObject>

namespace sigfwd
{
    connection::connection(QObject *qobj, result status) : 
        qobj(qobj),
        status(status) 
    {
    }

    connection::operator connection::boolean_type() const 
    {
        return status == connected ? &connection::status : 0; 
    }

} // close namespace sigfwd
