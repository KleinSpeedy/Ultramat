cmake_minimum_required(VERSION 3.16)

project(ultramat VERSION 0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER gcc)


# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

# Add compile flag
set(CMAKE_C_FLAGS "-rdynamic")

# Add other flags to the compiler
add_definitions(${GTK3_CFLAGS_OTHER})

# Add the executable from *.c files
add_executable(ultramat main.c position.c)

# TODO: target_compile_options doesnt work, fix with CMAKE_C_FLAGS
#target_compile_options(ultramat PRIVATE -rdynamic)

# Link the target to the GTK+ libraries
target_link_libraries(ultramat ${GTK3_LIBRARIES})
