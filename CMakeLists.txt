cmake_minimum_required (VERSION 3.8.2 FATAL_ERROR)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
project (ProgrammerStenoTutor)

# The application version number
set (PROJECT_VERSION_MAJOR 0)
set (PROJECT_VERSION_MINOR 1)
set (PROJECT_VERSION_PATCH 0)
 
# configure a header file to pass some of the CMake settings
# to the source code
configure_file ("${PROJECT_SOURCE_DIR}/ProjectConfig.h.in"
                "${PROJECT_BINARY_DIR}/ProjectConfig.h")

# Add the binary tree to the search path for include files
# so that we will find ${PROJECT_NAME}.h
include_directories("${PROJECT_BINARY_DIR}")

mark_as_advanced(CMAKE_INSTALL_PREFIX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Boost configuration
# Note: If there is a configuration error you may need to add 1 or 2 entries in cmake-gui : BOOST_ROOT and/or BOOST_LIBRARYDIR
# eg: BOOST_ROOT = C:/boost_1_64_0
# eg: BOOST_LIBRARYDIR = C:/boost_1_64_0/stage_x64/lib
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.64.0 REQUIRED COMPONENTS serialization)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

# Qt5 directory looks like this C:\Qt\5.9.1\msvc2015_64\lib\cmake\Qt5
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

file(GLOB SOURCES ./*.cpp)
file(GLOB QRCS ./*.qrc)
file(GLOB RCS ./*.rc)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${QRCS} ${RCS})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets)

if (Boost_FOUND)
    target_link_libraries(${PROJECT_NAME} Boost::serialization)
endif()
