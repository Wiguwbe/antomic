# Antomic Engine Launcher
FILE(GLOB_RECURSE LAUNCHER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE(GLOB_RECURSE LAUNCHER_HDR "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_executable("${PROJECT_NAME}Launcher" ${LAUNCHER_HDR} ${LAUNCHER_SRC})
target_compile_features("${PROJECT_NAME}Launcher" PUBLIC cxx_std_17)
target_include_directories( 
    "${PROJECT_NAME}Launcher"
    PRIVATE
    "${PROJECT_SOURCE_DIR}/engine"
    "${GLM_DIR}"
)


target_link_libraries(
    "${PROJECT_NAME}Launcher" 
    "${PROJECT_NAME}Engine"
    "stb"
    ${OPENGL_LIBRARIES} 
)

