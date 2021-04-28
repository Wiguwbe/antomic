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

# PThreads
target_link_libraries( 
    "${PROJECT_NAME}Engine"
    ${CMAKE_THREAD_LIBS_INIT}    
)

# SDL 
if (SDL_SUPPORT)
    target_link_libraries( 
        "${PROJECT_NAME}Engine"
        SDL2::Main
    )
ENDIF()

# GLFW 
if (GLFW_SUPPORT)
    target_link_libraries( 
        "${PROJECT_NAME}Engine"
        glfw
    )
ENDIF()

# OpenGL
target_include_directories( 
    "${PROJECT_NAME}Engine"
    PRIVATE
    ${OPENGL_INCLUDE_DIRS}
)
target_link_libraries(
    "${PROJECT_NAME}Engine"
    ${OPENGL_LIBRARIES} 
)

#JSON Library setup
target_link_libraries( 
    "${PROJECT_NAME}Engine"
    nlohmann_json::nlohmann_json
)

#Logging Library setup
target_include_directories( 
    "${PROJECT_NAME}Engine"
    PRIVATE
    ${LOG_INCLUDE_DIR}
)
target_link_libraries( 
    "${PROJECT_NAME}Engine"
    spdlog::spdlog
)

#Math Library setup
target_include_directories( 
    "${PROJECT_NAME}Engine"
    PRIVATE
    "${GLM_DIR}"
)

# GLAD 
target_include_directories( 
    "${PROJECT_NAME}Engine"
    PRIVATE
    "${GLAD_INCLUDE_DIR}"
)
target_link_libraries(
    "${PROJECT_NAME}Engine" 
    "glad"
)

# STB 
target_include_directories( 
    "${PROJECT_NAME}Engine"
    PRIVATE
    "${STB_INCLUDE_DIR}"
)
