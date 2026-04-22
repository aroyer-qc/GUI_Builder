cmake_minimum_required(VERSION 3.16)

project(GUI_Builder VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

# ---------------------------------------------------------
# FreeType (portable)
# ---------------------------------------------------------
set(FREETYPE_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/freetype/include/")
set(FREETYPE_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/freetype/freetype.lib")
include_directories(${FREETYPE_INCLUDE_DIRS})

# ---------------------------------------------------------
# Qt portable (Qt5 ou Qt6 selon le kit)
# ---------------------------------------------------------
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Xml)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Xml)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core5Compat)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Multimedia)

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
        editdefine.ui
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
# Executable (Qt5/Qt6 compatible)
# ---------------------------------------------------------
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(GUI_Builder
        SkinRessource.qrc
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(GUI_Builder
        ${PROJECT_SOURCES}
    )
endif()

# ---------------------------------------------------------
# Linking
# ---------------------------------------------------------
target_link_libraries(GUI_Builder PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Xml
    Qt${QT_VERSION_MAJOR}::Core5Compat
    Qt${QT_VERSION_MAJOR}::Multimedia
    ${FREETYPE_LIBRARIES}
)

set_target_properties(GUI_Builder PROPERTIES
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(GUI_Builder)
endif()
