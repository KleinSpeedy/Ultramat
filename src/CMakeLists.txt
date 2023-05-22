# Source level CMake File
add_executable(${PROJECT_NAME}
        main.c gui.c pages.c drinks.c drinklists.c callbacks.c serialcom.c ingredient.c recipe.c)

# Link the target to the GTK+ libraries and add compile options for
target_link_libraries(${PROJECT_NAME} PRIVATE ${GTK3_LIBRARIES} periphery pthread)

# Link to c-periphery lib headers
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/ext/c-periphery/src)

# Add compile options to target
target_compile_options(${PROJECT_NAME} PRIVATE ${ULTRAMAT_OPTIONS})

# TODO: Add Debug build option