find_package( Qt5Widgets REQUIRED )
find_package( Qt5Network REQUIRED )
find_package( Qt5PrintSupport REQUIRED )

set( QT5_PLUGINS )
if ( PLATFORM_WIN )
	set( QT5_PLUGINS Qt5::QWindowsIntegrationPlugin )
endif ()