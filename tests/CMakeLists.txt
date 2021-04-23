file(GLOB_RECURSE TESTS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE TESTS_HDR "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

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
