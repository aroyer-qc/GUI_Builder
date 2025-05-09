cmake_minimum_required(VERSION 3.5)

project(GUI_Builder VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

# Manually set FreeType paths if find_package fails
set(FREETYPE_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/freetype/include/")
set(FREETYPE_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/freetype/freetype.lib")
include_directories(${FREETYPE_INCLUDE_DIRS})
find_library(FREETYPE_LIB freetype PATHS ${FREETYPE_LIBRARIES})
find_package(REQUIRED COMPONENTS freetype)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Xml)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Xml)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core5Compat REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)


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

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(GUI_Builder
        SkinRessource.qrc
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET GUI_Builder APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(GUI_Builder SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(GUI_Builder
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(GUI_Builder PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Xml)
target_link_libraries(GUI_Builder PUBLIC Qt6::Core5Compat)
target_link_libraries(GUI_Builder PRIVATE Qt6::Multimedia)
# Link FreeType library
#link_directories("${PROJECT_SOURCES}/freetype/")
target_link_libraries(GUI_Builder PRIVATE ${FREETYPE_LIBRARIES})

set_target_properties(GUI_Builder PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS GUI_Builder
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(GUI_Builder)
endif()
