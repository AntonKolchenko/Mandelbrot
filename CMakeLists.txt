cmake_minimum_required(VERSION 3.5)

project(mandelbrot-AntonKolchenko VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        worker.cpp
        worker.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(mandelbrot-AntonKolchenko
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()

add_executable(mandelbrot-AntonKolchenko
    ${PROJECT_SOURCES}
)

endif()

target_link_libraries(mandelbrot-AntonKolchenko PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(mandelbrot-AntonKolchenko PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(mandelbrot-AntonKolchenko)
endif()

#add_compile_options(-fsanitize=address)
#add_link_options(-fsanitize=address)
#add_compile_options(-fsanitize=undefined)
#add_link_options(-fsanitize=undefined)
#add_compile_options(-fsanitize=thread)
#add_link_options(-fsanitize=thread)

##target_compile_options(mandelbrot-AntonKolchenko PRIVATE -fsanitize=address)
#target_compile_options(mandelbrot-AntonKolchenko PRIVATE -fsanitize=undefined)
#target_compile_options(mandelbrot-AntonKolchenko PRIVATE -fsanitize=thread)
##target_link_options(mandelbrot-AntonKolchenko PRIVATE -fsanitize=address)
#target_link_options(mandelbrot-AntonKolchenko PRIVATE -fsanitize=undefined)
#target_link_options(mandelbrot-AntonKolchenko PRIVATE -fsanitize=thread)
