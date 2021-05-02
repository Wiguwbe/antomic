file(GLOB_RECURSE TESTS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE(GLOB_RECURSE TESTS_HDRS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_executable("${PROJECT_NAME}GraphTests" ${TESTS_SRCS} ${TESTS_HDRS})

# Antomic Engine
target_include_directories( 
    "${PROJECT_NAME}GraphTests"
    PRIVATE
    "${PROJECT_SOURCE_DIR}/engine"
)

target_link_libraries(
    "${PROJECT_NAME}GraphTests" 
    PUBLIC 
    "${PROJECT_NAME}Engine"
)

# GLM Library setup
# https://github.com/g-truc/glm
target_include_directories( 
    "${PROJECT_NAME}GraphTests"
    PRIVATE
    "${GLM_DIR}"
)

# Unit Testing
# https://github.com/google/googletest.git
target_link_libraries(
    "${PROJECT_NAME}GraphTests" 
    PUBLIC 
    gtest_main
)


# Logging Library setup
# https://github.com/gabime/spdlog.git
target_link_libraries( 
    "${PROJECT_NAME}GraphTests"
    PRIVATE
    nlohmann_json::nlohmann_json
)

# Logging Library setup
# https://github.com/gabime/spdlog.git
target_link_libraries( 
    "${PROJECT_NAME}GraphTests"
    PRIVATE
    spdlog::spdlog
)