# Antoming Engine Editor
FILE(GLOB_RECURSE EDITOR_SRC "${PROJECT_SOURCE_DIR}/editor/*.cpp")
FILE(GLOB_RECURSE EDITOR_HDR "${PROJECT_SOURCE_DIR}/editor/*.h")
add_executable("${PROJECT_NAME}Editor" ${EDITOR_HDR} ${EDITOR_SRC})
target_compile_features("${PROJECT_NAME}Editor" PUBLIC cxx_std_17)
target_include_directories( 
    "${PROJECT_NAME}Editor"
    PRIVATE
    "${PROJECT_SOURCE_DIR}/engine"
)
target_link_libraries(
    "${PROJECT_NAME}Editor" 
    "${PROJECT_NAME}Engine"
)

target_link_libraries(
    "${PROJECT_NAME}Editor" 
    ${OPENGL_LIBRARIES} 
)


#ImGui Library Setup
target_include_directories( 
    "${PROJECT_NAME}Editor"
    PRIVATE
    ${IMGUI_DIR}
)
target_link_libraries(
    "${PROJECT_NAME}Editor" 
    "imgui"
)

#Math Library setup
target_include_directories( 
    "${PROJECT_NAME}Editor"
    PRIVATE
    "${GLM_DIR}"
)
