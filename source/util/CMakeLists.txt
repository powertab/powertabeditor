project( pteutil )

set( srcs
    rapidjson_iostreams.cpp
    settingstree.cpp
)

set( headers
    rapidjson_iostreams.h
    settingstree.h
)

pte_library(
    NAME pteutil
    SOURCES ${srcs}
    HEADERS ${headers}
    DEPENDS
        boost
        rapidjson
)
