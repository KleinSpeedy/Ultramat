cmake_minimum_required(VERSION 3.22)

if(${CMAKE_VERSION} VERSION_LESS 3.22)
    message("Using older CMake version than 3.22")
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.{CMAKE_MINOR_VERSION})
endif()

project(ultramat
    LANGUAGES
    C)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    # Project Settings
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(CMAKE_SYSTEM_NAME Linux)

    # Specify source and include directory
    set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
    set(HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/inc")

    # Collect all .c files in the source directory
    file(GLOB SOURCE_FILES "${SOURCE_DIR}/*.c")

    # Collect all header files in the include directory
    file(GLOB HEADER_FILES "${HEADER_DIR}/*.h")

    if(ULTRA_BUILD_DEBUG)
        message("Debug build")
        # Compiler options release
        set(ULTRAMAT_OPTIONS
            "-Og"
            "-Wall"
            "-Wextra"
            "-g")
        set(ULTRAMAT_DEFS
            "-DGTK_DEBUG=interactive"
            "-DG_DEBUG=fatal-warnings"
            CACHE STRING "")
        add_definitions("-DGTK_DEBUG")
    else()
        message("release build")
        # Compiler options release
        set(ULTRAMAT_OPTIONS
            "-O3"
            "-Wall"
            "-Wextra")
    endif()

    # Resources
    set(RES
        ./res/ingredients.txt
        ./res/recipes.txt
        ./res/style.css)

    # Let's nicely support folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

# c-periphery library
# dont build tests
set(BUILD_TESTS OFF CACHE BOOL "Build tests")
add_subdirectory(ext/c-periphery)

# Release Target
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

# Link the target to the GTK+ libraries (pthread gets linked through GTK3)
target_link_libraries(${PROJECT_NAME} PRIVATE ${GTK3_LIBRARIES} periphery)

# Link to c-periphery lib headers
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/ext/c-periphery/src ${HEADER_DIR})
# Add compile options to target
target_compile_options(${PROJECT_NAME} PRIVATE ${ULTRAMAT_OPTIONS})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${ULTRAMAT_DEFS})
