cmake_minimum_required(VERSION 3.10)
project(mellstroy_game)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск Raylib
find_package(raylib QUIET)
if(NOT raylib_FOUND)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(RAYLIB QUIET raylib)
        if(RAYLIB_FOUND)
            message(STATUS "Found raylib via pkg-config")
        endif()
    endif()
    
    if(NOT RAYLIB_FOUND)
        message(STATUS "Raylib not found, downloading via FetchContent...")
        include(FetchContent)
        FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 5.0
        )
        FetchContent_MakeAvailable(raylib)
    endif()
endif()

# Исходные файлы
set(SOURCES
    src/main.cpp
    src/game.cpp
    src/menu.cpp
)

# Заголовочные файлы
set(HEADERS
    src/game.h
)

# Создание исполняемого файла
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Подключение Raylib
if(RAYLIB_FOUND)
    target_include_directories(${PROJECT_NAME} PRIVATE ${RAYLIB_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${RAYLIB_LIBRARIES})
    target_compile_options(${PROJECT_NAME} PRIVATE ${RAYLIB_CFLAGS_OTHER})
else()
    target_link_libraries(${PROJECT_NAME} raylib)
endif()

# Установка рабочей директории для ресурсов
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

