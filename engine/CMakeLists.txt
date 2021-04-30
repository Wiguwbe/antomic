# Antomic Engine Library
FILE(GLOB_RECURSE ANTOMIC_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE(GLOB_RECURSE ANTOMIC_HDR "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_library("${PROJECT_NAME}Engine" ${ANTOMIC_HDR} ${ANTOMIC_SRC})
target_compile_features("${PROJECT_NAME}Engine" PUBLIC cxx_std_17)
target_include_directories( 
    "${PROJECT_NAME}Engine"
    PRIVATE
    "${PROJECT_SOURCE_DIR}/engine"
)

# POSIX Threads
target_link_libraries( 
    "${PROJECT_NAME}Engine"
    ${CMAKE_THREAD_LIBS_INIT}    
)

# SDL - Simple DirectMedia Layer
# https://www.libsdl.org/
if (SDL_SUPPORT)
    target_link_libraries( 
        "${PROJECT_NAME}Engine"
        SDL2::Main
    )
ENDIF(SDL_SUPPORT)

# GLFW - multi-platform library for OpenGL, OpenGL ES and Vulkan development on the desktop
# https://www.glfw.org/
if (GLFW_SUPPORT)
    target_link_libraries( 
        "${PROJECT_NAME}Engine"
        glfw
    )
ENDIF(GLFW_SUPPORT)

# OpenGL RenderAPI Support
# https://www.opengl.org/
if (OPENGL_SUPPORT)
    target_include_directories( 
        "${PROJECT_NAME}Engine"
        PRIVATE
        ${OPENGL_INCLUDE_DIRS}
    )
    target_link_libraries(
        "${PROJECT_NAME}Engine"
        ${OPENGL_LIBRARIES} 
    )
endif(OPENGL_SUPPORT)

# JSON Library setup
# https://github.com/nlohmann/json.git
target_link_libraries( 
    "${PROJECT_NAME}Engine"
    nlohmann_json::nlohmann_json
)

# Logging Library setup
# https://github.com/gabime/spdlog.git
target_include_directories( 
    "${PROJECT_NAME}Engine"
    PRIVATE
    ${LOG_INCLUDE_DIR}
)
target_link_libraries( 
    "${PROJECT_NAME}Engine"
    spdlog::spdlog
)

# GLM Library setup
# https://github.com/g-truc/glm
target_include_directories( 
    "${PROJECT_NAME}Engine"
    PRIVATE
    "${GLM_DIR}"
)

# GLAD Multi-Language GL/GLES/EGL/GLX/WGL Loader-Generator based on the official specs.
# https://glad.dav1d.de/
target_include_directories( 
    "${PROJECT_NAME}Engine"
    PRIVATE
    "${GLAD_INCLUDE_DIR}"
)
target_link_libraries(
    "${PROJECT_NAME}Engine" 
    "glad"
)
