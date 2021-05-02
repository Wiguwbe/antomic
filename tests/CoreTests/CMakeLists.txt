file(GLOB_RECURSE TESTS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable("${PROJECT_NAME}CoreTests" ${TESTS_SRCS})

# Antomic Engine
target_include_directories( 
    "${PROJECT_NAME}CoreTests"
    PRIVATE
    "${PROJECT_SOURCE_DIR}/engine"
)

target_link_libraries(
    "${PROJECT_NAME}CoreTests" 
    PUBLIC 
    "${PROJECT_NAME}Engine"
)

# GLM Library setup
# https://github.com/g-truc/glm
target_include_directories( 
    "${PROJECT_NAME}CoreTests"
    PRIVATE
    "${GLM_DIR}"
)

# Unit Testing
# https://github.com/google/googletest.git
target_link_libraries(
    "${PROJECT_NAME}CoreTests" 
    PUBLIC 
    gtest_main
)

# Logging Library setup
# https://github.com/gabime/spdlog.git
target_link_libraries( 
    "${PROJECT_NAME}CoreTests"
    PRIVATE
    nlohmann_json::nlohmann_json
)

# Logging Library setup
# https://github.com/gabime/spdlog.git
target_link_libraries( 
    "${PROJECT_NAME}CoreTests"
    PRIVATE
    spdlog::spdlog
)