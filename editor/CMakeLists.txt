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

target_link_libraries(
    "${PROJECT_NAME}Editor" 
    PRIVATE
    "${PROJECT_NAME}Engine"
)

# OpenGL RenderAPI Support
# https://www.opengl.org/
if(OPENGL_SUPPORT)
    target_link_libraries(
        "${PROJECT_NAME}Launcher"
        PRIVATE
        OpenGL::OpenGL
    )
endif() 

# GLM Library setup
# https://github.com/g-truc/glm
target_include_directories( 
    "${PROJECT_NAME}Editor"
    PRIVATE
    "${GLM_DIR}"
)

# single-file public domain (or MIT licensed) libraries for C/C++
# https://github.com/nothings/stb
target_link_libraries(
    "${PROJECT_NAME}Editor" 
    PRIVATE
    "stb"
)

# ImGui Include
target_include_directories( 
    "${PROJECT_NAME}Editor"
    PRIVATE
    ${IMGUI_DIR}
)


# ImGui Library
target_link_libraries(
    "${PROJECT_NAME}Editor" 
    PRIVATE
    "imgui"
)

# Logging Library setup
# https://github.com/gabime/spdlog.git
target_link_libraries( 
    "${PROJECT_NAME}Editor"
    PRIVATE
    spdlog::spdlog
)

# Logging Library setup
# https://github.com/gabime/spdlog.git
target_link_libraries( 
    "${PROJECT_NAME}Editor"
    PRIVATE
    nlohmann_json::nlohmann_json
)
