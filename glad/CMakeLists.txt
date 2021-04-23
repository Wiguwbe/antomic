# GLAD 
set(GLAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories(
    "glad" 
    PRIVATE 
    "${GLAD_DIR}/include"
)

# GLAD Platform specific configuration
if (UNIX)
    target_link_libraries(
        "glad"
        ${CMAKE_DL_LIBS}
    )
endif(UNIX)
