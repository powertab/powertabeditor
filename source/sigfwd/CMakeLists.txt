cmake_minimum_required(VERSION 2.8.8)

add_library(sigfwd
    connection.cpp
    signal_forwarder.cpp
    sig_string.cpp
)

qt5_use_modules(sigfwd Core) 
