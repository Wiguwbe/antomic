# Antoming Engine Editor
FILE(GLOB_RECURSE EDITOR_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE(GLOB_RECURSE EDITOR_HDR "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_executable("${PROJECT_NAME}Editor" ${EDITOR_HDR} ${EDITOR_SRC})
target_compile_features("${PROJECT_NAME}Editor" PUBLIC cxx_std_17)

# Antomic Engine Include
target_include_directories( 
    "${PROJECT_NAME}Editor"
    PRIVATE
    "${PROJECT_SOURCE_DIR}/engine"
)

# GLM Include
target_include_directories( 
    "${PROJECT_NAME}Editor"
    PRIVATE
    "${GLM_DIR}"
)

# ImGui Include
target_include_directories( 
    "${PROJECT_NAME}Editor"
    PRIVATE
    ${IMGUI_DIR}
)

# Antomic Engine Library
target_link_libraries(
    "${PROJECT_NAME}Editor" 
    "${PROJECT_NAME}Engine"
)

# OpenGL Library
target_link_libraries(
    "${PROJECT_NAME}Editor" 
    ${OPENGL_LIBRARIES} 
)

# ImGui Library
target_link_libraries(
    "${PROJECT_NAME}Editor" 
    "imgui"
)

