cmake_minimum_required(VERSION 3.5)

project(Game)

project(${PROJECT_NAME} CXX C)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_RELEASE 1)
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_RELEASE}")

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_definitions(-DENGINE_DEBUG)
endif()

find_package(Threads REQUIRED)
if(WIN32)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -std=c++17")
    add_definitions(-DENGINE_PLATFORM_WINDOWS)
elseif (UNIX)

    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -Werror -std=c++17")
    
    add_definitions(-DENGINE_PLATFORM_LINUX)

    find_package(PkgConfig REQUIRED)

    pkg_check_modules(SDL2 sdl2)

    if (SDL2_FOUND)
        add_definitions(-DENGINE_SDL_PLATFORM)
    else(SDL2_FOUND)
        message(ERROR "We need SDL2 for the moment")
    endif(SDL2_FOUND)

    pkg_check_modules(GL REQUIRED opengl)
    pkg_check_modules(X11 REQUIRED x11)
endif(WIN32)

set(VENDOR_DIR "${PROJECT_SOURCE_DIR}/vendor")

# Unit Testing
if(NOT DEFINED ENV{ENGINE_NO_TESTS})
    set(GOOGLE_TEST_DIR "${VENDOR_DIR}/googletest")
    add_subdirectory(${GOOGLE_TEST_DIR})
endif() 

# BGFX setup
set(BX_DIR "${VENDOR_DIR}/bx")
set(BGFX_DIR "${VENDOR_DIR}/bgfx")
set(BIMG_DIR "${VENDOR_DIR}/bim")
set(BX_INCLUDE_DIRS "${BX_DIR}/include")
set(BGFX_INCLUDE_DIRS "${BGFX_DIR}/include")
set(BIMG_INCLUDE_DIRS "${BIMG_DIR}/include")

#JSON Library setup
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_DIR "${VENDOR_DIR}/json")
set(JSON_INCLUDE_DIR "${JSON_DIR}/json/single_include")

add_subdirectory(${JSON_DIR})

#Logging Library setup
set(LOG_DIR "${VENDOR_DIR}/spdlog")
set(LOG_INCLUDE_DIR "${LOG_DIR}/include")

add_subdirectory(${LOG_DIR})

if (UNIX)
    set(BGFX_LIBRARY "${BGFX_DIR}/.build/linux64_gcc/bin/libbgfx-shared-libRelease.so")
endif(UNIX)

# Specify build paths
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/dist")

#ImGui Library Setup
set(IMGUI_DIR "${VENDOR_DIR}/imgui")
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

FILE(GLOB_RECURSE ENGINE_SRC "${PROJECT_SOURCE_DIR}/engine/*.cpp")
FILE(GLOB_RECURSE ENGINE_HDR "${PROJECT_SOURCE_DIR}/engine/*.h")

add_library("${PROJECT_NAME}Engine" ${ENGINE_HDR} ${ENGINE_SRC})
target_include_directories( 
    "${PROJECT_NAME}Engine"
    PRIVATE
    "${PROJECT_SOURCE_DIR}/engine"
    ${LOG_INCLUDE_DIR}
    ${JSON_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIRS}
    ${GL_INCLUDE_DIRS}
    ${X11_INCLUDE_DIRS}
    ${BX_INCLUDE_DIRS}
    ${BGFX_INCLUDE_DIRS}
    ${BIMG_INCLUDE_DIRS}
)
target_link_libraries( 
    "${PROJECT_NAME}Engine"
    ${BGFX_LIBRARY}
    ${SDL2_LIBRARIES} 
    ${GL_LIBRARIES} 
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
)

FILE(GLOB_RECURSE EDITOR_SRC "${PROJECT_SOURCE_DIR}/editor/*.cpp")
FILE(GLOB_RECURSE EDITOR_HDR "${PROJECT_SOURCE_DIR}/editor/*.h")
add_executable("${PROJECT_NAME}Editor" ${EDITOR_HDR} ${EDITOR_SRC})
target_include_directories( 
    "${PROJECT_NAME}Editor"
    PRIVATE
    "${PROJECT_SOURCE_DIR}/engine"
    ${BX_INCLUDE_DIRS}
    ${BGFX_INCLUDE_DIRS}
   ${IMGUI_DIR}
)
target_link_libraries(
    "${PROJECT_NAME}Editor" 
    "${PROJECT_NAME}Engine"
    "imgui"
)

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
        gtest
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

if(UNIX)
    install(FILES ${BGFX_LIBRARY}
        DESTINATION ${CMAKE_INSTALL_PREFIX}
        CONFIGURATIONS Debug Release
    )
endif(UNIX)
