cmake_minimum_required(VERSION 3.16)

project(GUI_Builder VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

# ---------------------------------------------------------
# FreeType: auto-detect MSVC vs MinGW
# ---------------------------------------------------------
if (MSVC)
    message(STATUS "Compiler: MSVC → Using 'Freetype - MSVC'")
    set(FREETYPE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/Freetype - MSVC")
elseif (MINGW)
    message(STATUS "Compiler: MinGW → Using 'Freetype'")
    set(FREETYPE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/Freetype")
else()
    message(FATAL_ERROR "Unsupported compiler for FreeType setup")
endif()

set(FREETYPE_INCLUDE_DIR "${FREETYPE_ROOT}/include")
set(FREETYPE_LIBRARY "${FREETYPE_ROOT}/freetype.lib")

include_directories(${FREETYPE_INCLUDE_DIR})
link_directories(${FREETYPE_ROOT})

# ---------------------------------------------------------
# Qt6 ONLY
# ---------------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Xml
    Core5Compat
    Multimedia
)

# ---------------------------------------------------------
# Sources
# ---------------------------------------------------------
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    AddingAudio.cpp
    AddingAudio.h
    AddingAudio.ui
    AddingImage.cpp
    AddingImage.h
    AddingImage.ui
    Audio.cpp
    Configuration.cpp
    ComboBoxDelegate.cpp
    ComboBoxDelegate.h
    compression.cpp
    compression.h
    Converter.cpp
    file.cpp
    Font.cpp
    Image.cpp
    ImageSpinBox.cpp
    ImageSpinBox.h
    lzw.cpp
    option.cpp
    progress.cpp
    progress.h
    progress.ui
    qxmlputget.cpp
    qxmlputget.h
    Simulator.cpp
    SkinOpen.cpp
    SkinOpen.h
    SkinSave.cpp
    SkinSave.h
    SpinBoxDelegate.cpp
    SpinBoxDelegate.h
    typedef.h
    Utility.cpp
    Utility.h
    widget.cpp
    ressources.rc
)

# ---------------------------------------------------------
# Executable
# ---------------------------------------------------------
qt_add_executable(GUI_Builder
    SkinRessource.qrc
    ${PROJECT_SOURCES}
)

# ---------------------------------------------------------
# Linking
# ---------------------------------------------------------
target_link_libraries(GUI_Builder PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Xml
    Qt6::Core5Compat
    Qt6::Multimedia
    ${FREETYPE_LIBRARY}
)

# ---------------------------------------------------------
# Windows executable
# ---------------------------------------------------------
set_target_properties(GUI_Builder PROPERTIES
    WIN32_EXECUT