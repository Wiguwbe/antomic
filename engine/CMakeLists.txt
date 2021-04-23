# Antomic Engine Library
FILE(GLOB_RECURSE ENGINE_SRC "${PROJECT_SOURCE_DIR}/engine/*.cpp")
FILE(GLOB_RECURSE ENGINE_HDR "${PROJECT_SOURCE_DIR}/engine/*.h")
add_library("${PROJECT_NAME}Engine" ${ENGINE_HDR} ${ENGINE_SRC})
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
target_link_libraries( 
    "${PROJECT_NAME}Engine"
    SDL2::Main
)

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
    "${GLAD_DIR}/include"
)
target_link_libraries(
    "${PROJECT_NAME}Engine" 
    "glad"
)
