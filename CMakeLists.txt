cmake_minimum_required (VERSION 3.8.2 FATAL_ERROR)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
project (ProgrammerStenoTutor)

mark_as_advanced(CMAKE_INSTALL_PREFIX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Note: cmake qt packages can be found here: C:\Qt\5.9.1\msvc2015_64\lib\cmake\Qt5*
find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5Widgets)

file(GLOB SOURCES ./*.cpp)
file(GLOB QRCS ./*.qrc)
file(GLOB RCS ./*.rc)

add_executable(ProgrammerStenoTutor WIN32 ${SOURCES} ${QRCS} ${RCS})
target_link_libraries(ProgrammerStenoTutor Qt5::Core Qt5::Gui Qt5::Widgets)
