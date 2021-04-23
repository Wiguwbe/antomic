file(GLOB_RECURSE TESTS_SRCS "${PROJECT_SOURCE_DIR}/tests/*.cpp")
file(GLOB_RECURSE TESTS_HDR "${PROJECT_SOURCE_DIR}/tests/*.h")

add_executable("${PROJECT_NAME}Tests" ${TESTS_SRCS} ${TESTS_HDR})

add_test(
    NAME "${PROJECT_NAME}Tests" 
    COMMAND "${PROJECT_NAME}Tests"
)

target_include_directories( 
    "${PROJECT_NAME}Tests"
    PRIVATE
    "${PROJECT_SOURCE_DIR}/engine"
)

target_link_libraries(
    "${PROJECT_NAME}Tests" 
    PUBLIC 
    "${PROJECT_NAME}Engine"
    gtest_main
)
