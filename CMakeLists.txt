cmake_minimum_required(VERSION 3.5)

project(Antomic)

project(${PROJECT_NAME} CXX C)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_RELEASE 1)
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_RELEASE}")
set(VENDOR_DIR "${PROJECT_SOURCE_DIR}/vendor")

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_definitions(-DENGINE_DEBUG)
endif()

find_package(Threads REQUIRED)

# Support both 32 and 64 bit builds
if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
    set(BUILD_ARCH "x64")
else ()
    set(BUILD_ARCH "x86")
endif ()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

if(WIN32)
    add_definitions(-DENGINE_PLATFORM_WINDOWS)
elseif (UNIX)
    add_definitions(-DENGINE_PLATFORM_LINUX)
endif(WIN32)

# SDL 
add_definitions(-DENGINE_SDL_PLATFORM)
find_package(SDL2 REQUIRED)

# OpenGL
add_definitions(-DENGINE_GL_RENDERER)
if (UNIX)
    set("OpenGL_GL_PREFERENCE" "GLVND")
    pkg_check_modules(X11 REQUIRED x11)
endif(UNIX)
find_package(OpenGL REQUIRED)

# Unit Testing
if(NOT DEFINED ENV{ENGINE_NO_TESTS})
    # Download and unpack googletest at configure time
    configure_file(CMakeLists.googletest.in googletest-download/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
    if(result)
        message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
    if(result)
        message(FATAL_ERROR "Build step for googletest failed: ${result}")
    endif()

    # Prevent overriding the parent project's compiler/linker
    # settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Add googletest directly to our build. This defines
    # the gtest and gtest_main targets.
    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                    ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                    EXCLUDE_FROM_ALL)

    # The gtest/gtest_main targets carry header search path
    # dependencies automatically when using CMake 2.8.11 or
    # later. Otherwise we have to add them here ourselves.
    if (CMAKE_VERSION VERSION_LESS 2.8.11)
        include_directories("${gtest_SOURCE_DIR}/include")
    endif()

endif() 

#JSON Library setup
configure_file(CMakeLists.json.in json-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
RESULT_VARIABLE result
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/json-download )
if(result)
    message(FATAL_ERROR "CMake step for json failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
RESULT_VARIABLE result
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/json-download )
if(result)
    message(FATAL_ERROR "Build step for json failed: ${result}")
endif()
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_DIR "${CMAKE_CURRENT_BINARY_DIR}/json-src")
add_subdirectory(${JSON_DIR})

#Logging Library setup
configure_file(CMakeLists.spdlog.in spdlog-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
RESULT_VARIABLE result
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/spdlog-download )
if(result)
    message(FATAL_ERROR "CMake step for spdlog failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
RESULT_VARIABLE result
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/spdlog-download )
if(result)
    message(FATAL_ERROR "Build step for spdlog failed: ${result}")
endif()
set(LOG_DIR "${CMAKE_CURRENT_BINARY_DIR}/spdlog-src")
set(LOG_INCLUDE_DIR "${LOG_DIR}/include")
add_subdirectory(${LOG_DIR})

#ImGui Library Setup
configure_file(CMakeLists.imgui.in imgui-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
RESULT_VARIABLE result
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/imgui-download )
if(result)
    message(FATAL_ERROR "CMake step for imgui failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
RESULT_VARIABLE result
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/imgui-download )
if(result)
    message(FATAL_ERROR "Build step for imgui failed: ${result}")
endif()
set(IMGUI_DIR "${CMAKE_CURRENT_BINARY_DIR}/imgui-src")
FILE(GLOB IMGUI_SRC "${IMGUI_DIR}/*.cpp")
FILE(GLOB IMGUI_HDR "${IMGUI_DIR}/*.h")
FILE(GLOB IMGUI_STL_SRC "${IMGUI_DIR}/misc/cpp/*.cpp")
FILE(GLOB IMGUI_STL_HDR "${IMGUI_DIR}/misc/cpp/*.h")
add_library("imgui" ${IMGUI_SRC} ${IMGUI_HDR})
add_library("imgui-stl" ${IMGUI_STL_SRC} ${IMGUI_STL_HDR})
target_include_directories( 
    "imgui-stl"
    PRIVATE
    "${IMGUI_DIR}"
)
target_link_libraries( 
    "imgui-stl"
    PRIVATE
    "imgui" 
)

#Math Library setup
configure_file(CMakeLists.glm.in glm-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
RESULT_VARIABLE result
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/glm-download )
if(result)
    message(FATAL_ERROR "CMake step for glm failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
RESULT_VARIABLE result
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/glm-download )
if(result)
    message(FATAL_ERROR "Build step for glm failed: ${result}")
endif()
set(GLM_DIR "${CMAKE_CURRENT_BINARY_DIR}/glm-src")
set(GLM_INCLUDE_DIR "${GLM_DIR}/glm")
add_subdirectory(${GLM_DIR})

# glad
set(GLAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")

# Specify build paths
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/dist")

FILE(GLOB_RECURSE ENGINE_SRC "${PROJECT_SOURCE_DIR}/engine/*.cpp")
FILE(GLOB_RECURSE ENGINE_HDR "${PROJECT_SOURCE_DIR}/engine/*.h")

add_library("${PROJECT_NAME}Engine" ${ENGINE_HDR} ${ENGINE_SRC})
target_include_directories( 
    "${PROJECT_NAME}Engine"
    PRIVATE
    "${PROJECT_SOURCE_DIR}/engine"
    ${LOG_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIRS}
    ${X11_INCLUDE_DIRS}
    "${GLAD_DIR}/include"
)
target_link_libraries( 
    "${PROJECT_NAME}Engine"
    SDL2::Main
    "glad"
    ${X11_LIBRARIES}
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    ${CMAKE_THREAD_LIBS_INIT}    
)

FILE(GLOB_RECURSE LAUNCHER_SRC "${PROJECT_SOURCE_DIR}/launcher/*.cpp")
FILE(GLOB_RECURSE LAUNCHER_HDR "${PROJECT_SOURCE_DIR}/launcher/*.h")
add_executable("${PROJECT_NAME}Launcher" ${LAUNCHER_HDR} ${LAUNCHER_SRC})
target_include_directories( 
    "${PROJECT_NAME}Launcher"
    PRIVATE
    "${PROJECT_SOURCE_DIR}/engine"
)
target_link_libraries(
    "${PROJECT_NAME}Launcher" 
    "${PROJECT_NAME}Engine"
    "imgui"
    ${OPENGL_LIBRARIES} 
)

FILE(GLOB_RECURSE EDITOR_SRC "${PROJECT_SOURCE_DIR}/editor/*.cpp")
FILE(GLOB_RECURSE EDITOR_HDR "${PROJECT_SOURCE_DIR}/editor/*.h")
add_executable("${PROJECT_NAME}Editor" ${EDITOR_HDR} ${EDITOR_SRC})
target_include_directories( 
    "${PROJECT_NAME}Editor"
    PRIVATE
    "${PROJECT_SOURCE_DIR}/engine"
    ${IMGUI_DIR}
)
target_link_libraries(
    "${PROJECT_NAME}Editor" 
    "${PROJECT_NAME}Engine"
    "imgui"
    ${OPENGL_LIBRARIES} 
)

target_compile_features("${PROJECT_NAME}Launcher" PUBLIC cxx_std_17)
target_compile_features("${PROJECT_NAME}Editor" PUBLIC cxx_std_17)
target_compile_features("${PROJECT_NAME}Engine" PUBLIC cxx_std_17)

if(NOT DEFINED ENV{ENGINE_NO_TESTS})
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
endif()

file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX})

install(TARGETS "${PROJECT_NAME}Engine"
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
    CONFIGURATIONS Debug Release
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/media
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    CONFIGURATIONS Debug Release
)

install(FILES ${PROJECT_SOURCE_DIR}/settings.json
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    CONFIGURATIONS Debug Release
)

