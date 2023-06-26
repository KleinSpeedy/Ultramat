# Source level CMake File
set(SRC_FILES
        "main.c" "gui.c" "pages.c" "drinks.c" "drinklists.c" "callbacks.c" "serialcom.c" "ingredient.c" "recipe.c")

# Release Target
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Link the target to the GTK+ libraries (pthread gets linked through GTK3)
target_link_libraries(${PROJECT_NAME} PRIVATE ${GTK3_LIBRARIES} periphery)

# Link to c-periphery lib headers
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/ext/c-periphery/src)
# Add compile options to target
target_compile_options(${PROJECT_NAME} PRIVATE ${ULTRAMAT_OPTIONS})
