# Antomic Engine Library
ASDL_TARGET(PYTHON_LANG ${CMAKE_CURRENT_SOURCE_DIR}/Script/grammar.asdl
    ${CMAKE_CURRENT_SOURCE_DIR}/Script/Parser/Ast.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Script/Parser/Ast.cpp)

FILE(GLOB_RECURSE ANTOMIC_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE(GLOB_RECURSE ANTOMIC_HDR "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_library("${PROJECT_NAME}Engine" ${ANTOMIC_HDR} ${ANTOMIC_SRC} ${ASDL_PYTHON_LANG_OUTPUTS})
target_compile_features("${PROJECT_NAME}Engine" PUBLIC cxx_std_17)
target_include_directories( 
    "${PROJECT_NAME}Engine"
    PRIVATE
    "${PROJECT_SOURCE_DIR}/engine"
)

# POSIX Threads
if(CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries( 
        "${PROJECT_NAME}Engine"
        PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}    
    )
endif()

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
        PRIVATE
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
    PRIVATE
    nlohmann_json::nlohmann_json
)

# Logging Library setup
# https://github.com/gabime/spdlog.git
target_link_libraries( 
    "${PROJECT_NAME}Engine"
    PRIVATE
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
    PRIVATE
    "glad"
)

# single-file public domain (or MIT licensed) libraries for C/C++
# https://github.com/nothings/stb
target_link_libraries(
    "${PROJECT_NAME}Engine"
    PRIVATE
    "stb"
)

