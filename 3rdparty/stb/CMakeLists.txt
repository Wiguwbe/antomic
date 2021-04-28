# STB 
set(STB_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(STB_INCLUDE_DIR "${GLAD_DIR}/include" PARENT_SCOPE)
add_library("stb" INTERFACE)
target_include_directories("stb" INTERFACE .)
