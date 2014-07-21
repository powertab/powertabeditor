cmake_minimum_required(VERSION 2.8.9)
project(powertabeditor)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
option(ENABLE_WERROR "Fail and stop if a warning is triggered." OFF)

include(cmake/cotire.cmake)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CMAKE_DEBUG_POSTFIX "d")

    add_definitions(-DBOOST_ALL_DYN_LINK)
    add_definitions(-DBOOST_ALL_NO_LIB)
    add_definitions(-DNOMINMAX)
    # Increase the virtual memory limit for precompiled headers.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm200")
else()
    # Set -Wno-ignored-qualifiers for boost::ptr_container (see https://svn.boost.org/trac/boost/ticket/4276).
    # Set -Wno-deprecated to silence noisy warnings from Qt headers that use the 'register' keyword.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wnon-virtual-dtor -Wextra -Wno-ignored-qualifiers -Wno-deprecated")
    if (ENABLE_WERROR)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    endif()
endif()

# Run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED) 

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    find_package(Boost REQUIRED COMPONENTS date_time iostreams zlib)
else()
    find_package(Boost REQUIRED COMPONENTS date_time iostreams)
    find_package(ZLIB REQUIRED)
endif()

# Find the ALSA package for Linux.
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(ALSA REQUIRED)
endif()

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/source)

add_subdirectory(source)
add_subdirectory(test)
