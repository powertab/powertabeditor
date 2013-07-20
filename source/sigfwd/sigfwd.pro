include (../../common.pri)

HEADERS += \
    connect.hpp \
    connection.hpp \
    functor_traits.hpp \
    sigfwd.hpp \
    type_registry.hpp \
    version.hpp \
    impl/connect_helpers.hpp \
    impl/signal_forwarder.hpp \
    impl/sig_string.hpp \
    impl/std_functor_support.hpp

SOURCES += \
    connection.cpp \
    signal_forwarder.cpp \
    sig_string.cpp
