find_package( pugixml REQUIRED )

# pugixml version 1.11 already defines the namespaced pugixml target.
if ( NOT TARGET pugixml::pugixml )
    add_library( pugixml::pugixml INTERFACE IMPORTED )
    target_link_libraries( pugixml::pugixml
        INTERFACE pugixml
    )
endif ()
