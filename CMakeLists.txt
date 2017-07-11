cmake_minimum_required (VERSION 3.5.1 FATAL_ERROR)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
project (ProgrammerStenoTutor)

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
find_package(Boost 1.58.0 REQUIRED COMPONENTS serialization)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

# Qt5 directory looks like this C:\Qt\5.9.1\msvc2015_64\lib\cmake\Qt5
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

file(GLOB SOURCES ./*.cpp)
file(GLOB QRCS ./*.qrc)
file(GLOB RCS ./*.rc)

add_executable(ProgrammerStenoTutor WIN32 ${SOURCES} ${QRCS} ${RCS})
target_link_libraries(ProgrammerStenoTutor Qt5::Core Qt5::Gui Qt5::Widgets)

if (Boost_FOUND)
    target_link_libraries(ProgrammerStenoTutor Boost::serialization)
endif()
