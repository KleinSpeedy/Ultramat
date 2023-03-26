cmake_minimum_required(VERSION 3.16)

project(ultramat VERSION 0.1.5)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    # Project Settings
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_C_FLAGS "-rdynamic")
    
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

# Add other flags to the compiler
add_definitions(${GTK3_CFLAGS_OTHER})

# Include Files
include_directories(inc)

# Source files
add_subdirectory(src)

# TODO: target_compile_options doesnt work, fix with CMAKE_C_FLAGS
#target_compile_options(ultramat PRIVATE -rdynamic)
