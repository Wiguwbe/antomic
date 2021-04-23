# Antomic Engine Launcher
FILE(GLOB_RECURSE LAUNCHER_SRC "${PROJECT_SOURCE_DIR}/launcher/*.cpp")
FILE(GLOB_RECURSE LAUNCHER_HDR "${PROJECT_SOURCE_DIR}/launcher/*.h")
add_executable("${PROJECT_NAME}Launcher" ${LAUNCHER_HDR} ${LAUNCHER_SRC})
target_compile_features("${PROJECT_NAME}Launcher" PUBLIC cxx_std_17)
target_include_directories( 
    "${PROJECT_NAME}Launcher"
    PRIVATE
    "${PROJECT_SOURCE_DIR}/engine"
)
target_link_libraries(
    "${PROJECT_NAME}Launcher" 
    "${PROJECT_NAME}Engine"
)

target_link_libraries(
    "${PROJECT_NAME}Launcher" 
    ${OPENGL_LIBRARIES} 
)

#Math Library setup
target_include_directories( 
    "${PROJECT_NAME}Launcher"
    PRIVATE
    "${GLM_DIR}"
)
