find_package( pugixml REQUIRED )

add_library( pugixml::pugixml INTERFACE IMPORTED )
target_link_libraries( pugixml::pugixml
    INTERFACE pugixml
)
