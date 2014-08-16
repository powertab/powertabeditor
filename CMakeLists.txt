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
option(ENABLE_COVERAGE "Enable code coverage." OFF)

include(cmake/cotire.cmake)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CMAKE_DEBUG_POSTFIX "d")

    add_definitions(-DBOOST_ALL_DYN_LINK)
    add_definitions(-DBOOST_ALL_NO_LIB)
    add_definitions(-DNOMINMAX)
    # Increase the virtual memory limit for precompiled headers.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm200")
else()
    # Set -Wno-deprecated to silence noisy warnings from Qt headers that use the 'register' keyword.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wnon-virtual-dtor -Wextra -Wno-deprecated")
    if (ENABLE_WERROR)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    endif()

    # Print coloured error messages when compiling with Clang and using Ninja.
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_GENERATOR STREQUAL "Ninja")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcolor-diagnostics")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
    endif()

    if (ENABLE_COVERAGE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    endif()
endif()

# Configure backends for RtMidi.
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions(-D__LINUX_ALSA__)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions(-D__MACOSX_AU__)
    add_definitions(-D__MACOSX_CORE__)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_definitions(-D__WINDOWS_MM__)
endif()

find_package(Qt5Widgets REQUIRED) 

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    find_package(Boost REQUIRED COMPONENTS date_time iostreams zlib program_options)
else()
    find_package(Boost REQUIRED COMPONENTS date_time iostreams program_options)
    find_package(ZLIB REQUIRED)
endif()

# Find the ALSA package for Linux.
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(ALSA REQUIRED)
endif()

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/source)
include_directories(${PROJECT_SOURCE_DIR}/external/rapidjson/include)
include_directories(${PROJECT_SOURCE_DIR}/external/pugixml/src)

add_subdirectory(external)
add_subdirectory(source)
add_subdirectory(test)
