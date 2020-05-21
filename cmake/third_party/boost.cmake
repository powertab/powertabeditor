# Creates the imported targets Boost::headers, Boost::date_time, etc
find_package(
    Boost 1.65 REQUIRED
    COMPONENTS
        date_time
        filesystem
        iostreams
        program_options
        system
)
