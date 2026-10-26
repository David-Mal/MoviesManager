cmake_minimum_required(VERSION 3.30)
project(MoviesManager)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

add_library(core STATIC
        src/domain/domain.cpp
        src/utils.cpp
        src/domain/MovieValidator.cpp
        src/repository/repository.cpp
        src/repository/file_repository.cpp
        src/repository/html_repository.cpp
        src/repository/csv_repository.cpp
        src/repository/db_repository.cpp
        src/repository/repository_factory.cpp
        src/sqlite3.c
        src/service/service.cpp
)

target_compile_definitions(core PRIVATE
        SEED_FILE_PATH="${CMAKE_SOURCE_DIR}/movies_seed.txt")

target_include_directories(core PUBLIC headers)

add_executable(MoviesManager
        main.cpp
        StartUp.cpp
        StartUp.h
        src/ui/ui.cpp
        src/tests/tests.cpp
)
target_link_libraries(MoviesManager PRIVATE core)

add_executable(MoviesGUI
        gui/gui_main.cpp
        gui/mainwindow.h gui/mainwindow.cpp gui/mainwindow.ui



)
target_include_directories(MoviesGUI PRIVATE gui)
target_link_libraries(MoviesGUI PRIVATE core Qt6::Core Qt6::Gui Qt6::Widgets)

if (WIN32)
    set_target_properties(MoviesGUI PROPERTIES WIN32_EXECUTABLE OFF)  # ON ascunde consola de depanare
endif()