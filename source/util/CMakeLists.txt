project( pteutil )

set( srcs
    rapidjson_iostreams.cpp
)

set( headers
    rapidjson_iostreams.h
)

pte_library(
    NAME pteutil
    SOURCES ${srcs}
    HEADERS ${headers}
    DEPENDS
        rapidjson
)
