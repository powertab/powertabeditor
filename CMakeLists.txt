cmake_minimum_required(VERSION 2.8.8)
project(powertabeditor)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(MSVC)
    add_definitions(-DBOOST_ALL_NO_LIB)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wnon-virtual-dtor -Wextra")
endif()

# Run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED) 
find_package(Boost REQUIRED COMPONENTS date_time iostreams serialization)

# Find the ALSA package for Linux.
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(ALSA REQUIRED)
endif()

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/source)

add_subdirectory(source)
add_subdirectory(test)
