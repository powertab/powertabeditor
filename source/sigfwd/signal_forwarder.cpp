// Copyright 2010 Edd Dawson.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "sigfwd/connect.hpp"

#include <cassert>

namespace sigfwd
{
    namespace impl
    {
        signal_forwarder::signal_forwarder(QObject *parent) :
            QObject(parent)
        {
        }

        connection signal_forwarder::connect(QObject *emitter, const char *qt_signal_sig, const char *recv_sig, 
                                             Qt::ConnectionType conn_type, bool check_sigs)
        {
            assert(emitter);
            assert(qt_signal_sig);

            // Qt's SIGNAL(xyz) creates a string that starts with a '2'. '1' would indicate a SLOT.
            assert(qt_signal_sig[0] == '2');
            qt_signal_sig++;
            
            const QByteArray norm_qt_sig = QMetaObject::normalizedSignature(qt_signal_sig);
            const QByteArray norm_recv_sig = QMetaObject::normalizedSignature(recv_sig);

            if (check_sigs && !QMetaObject::checkConnectArgs(norm_qt_sig, norm_recv_sig))
                return connection(this, sigs_incompatible);

            const int qt_signal_id = emitter->metaObject()->indexOfSignal(norm_qt_sig);
            if (qt_signal_id == -1)
                return connection(this, signal_not_found);
            
            // signals and slots in less-derived classes occupy id's that are lower than those
            // in more-derived classes. 
            // The first id in this class is therefore equal to the number of signal and slot 
            // methods in QObject (our base class).
            const int recv_slot_id = QObject::metaObject()->methodCount();
            QMetaObject::connect(emitter, qt_signal_id, this, recv_slot_id, conn_type);

            return connection(this, connected);
        }

        signal_forwarder::~signal_forwarder()
        {
        }

        int signal_forwarder::qt_metacall(QMetaObject::Call call, int id, void **ret_and_argv)
        {
            // Can the "meta call" be handled by the base class?
            int method_id = QObject::qt_metacall(call, id, ret_and_argv);
            if (method_id == -1 || call != QMetaObject::InvokeMetaMethod)
                return method_id; // this wasn't meant for our derived meta-caller.

            // method_id is equal to id less the number of meta-methods in our base-classes.
            // This gives us an index in to the meta-methods associated with our derived part.
            // We've only got a single meta-method, so this index should be 0.
            assert(method_id == 0);

            // The first pointer of ret_and_argv points to a location where the return value of the
            // meta-method would be stored. We're going to swallow the return value, so we only need
            // to pass on pointers to the arguments.
            void **argv = ret_and_argv + 1;

            try { forward(argv); }
            catch (...) { /**GULP**/ }

            return -1;
        }

    } // close namespace impl

} // close namespace sigfwd
