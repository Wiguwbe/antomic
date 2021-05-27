file(GLOB_RECURSE TESTS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable("${PROJECT_NAME}ScriptTests" ${TESTS_SRCS})

# Antomic Engine
target_include_directories( 
    "${PROJECT_NAME}ScriptTests"
    PRIVATE
    "${PROJECT_SOURCE_DIR}/engine"
)

target_link_libraries(
    "${PROJECT_NAME}ScriptTests" 
    PUBLIC 
    "${PROJECT_NAME}Engine"
)

# Unit Testing
# https://github.com/google/googletest.git
target_link_libraries(
    "${PROJECT_NAME}ScriptTests" 
    PUBLIC 
    gtest_main
)

# Logging Library setup
# https://github.com/gabime/spdlog.git
target_link_libraries( 
    "${PROJECT_NAME}ScriptTests"
    PRIVATE
    spdlog::spdlog
)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tests/files)

add_custom_command(
        TARGET "${PROJECT_NAME}ScriptTests" POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/tests/files/lexer_test.py
                ${CMAKE_BINARY_DIR}/tests/files/lexer_test.py)

add_custom_command(
        TARGET "${PROJECT_NAME}ScriptTests" POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/tests/files/parser_test.py
                ${CMAKE_BINARY_DIR}/tests/files/parser_test.py)                