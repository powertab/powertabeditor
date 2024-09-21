# Creates the imported targets Boost::headers, Boost::date_time, etc
find_package(
    Boost 1.74 CONFIG REQUIRED
    COMPONENTS
        date_time
        iostreams
)
