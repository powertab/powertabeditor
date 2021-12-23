# Find and configure third-party libraries.

set( PTE_EXTERNAL_FOLDER_NAME third_party )

include ( third_party/boost )
if ( BUILD_TESTING )
    include ( third_party/doctest )
endif ()
include ( third_party/minizip )
include ( third_party/nlohmann_json )
include ( third_party/pugixml )
include ( third_party/Qt )
include ( third_party/rtmidi )
