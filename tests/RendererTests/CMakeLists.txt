file(GLOB_RECURSE TESTS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable("${PROJECT_NAME}RendererTests" ${TESTS_SRCS})

# Antomic Engine
target_include_directories( 
    "${PROJECT_NAME}RendererTests"
    PRIVATE
    "${PROJECT_SOURCE_DIR}/engine"
)

target_link_libraries(
    "${PROJECT_NAME}RendererTests" 
    PUBLIC 
    "${PROJECT_NAME}Engine"
)

# GLM Library setup
# https://github.com/g-truc/glm
target_include_directories( 
    "${PROJECT_NAME}RendererTests"
    PRIVATE
    "${GLM_DIR}"
)

# Unit Testing
# https://github.com/google/googletest.git
target_link_libraries(
    "${PROJECT_NAME}RendererTests" 
    PUBLIC 
    gtest_main
)

