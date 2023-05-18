cmake_minimum_required(VERSION 3.16)

project(ultramat VERSION 0.1.5)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    # Project Settings
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(CMAKE_SYSTEM_NAME Linux)

    # Compiler options
    set(ULTRAMAT_OPTIONS
            "-O3"
            "-Wall"
            "-Wextra")

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

# Include Files
include_directories(inc)

# Source files
add_subdirectory(src)