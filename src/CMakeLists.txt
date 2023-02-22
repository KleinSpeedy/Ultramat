# Source level CMake File
add_executable(ultramat main.c gui.c pages.c drinks.c drinkarray.c drinklists.c)

# Link the target to the GTK+ libraries
target_link_libraries(ultramat PRIVATE ${GTK3_LIBRARIES})