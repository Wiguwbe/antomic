cmake_minimum_required(VERSION 3.5)

project(Antomic)

project(${PROJECT_NAME} CXX C)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_RELEASE 1)
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_RELEASE}")

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_definitions(-DANTOMIC_DEBUG)
endif()

# Support both 32 and 64 bit builds
if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
    set(BUILD_ARCH "x64")
else ()
    set(BUILD_ARCH "x86")
endif ()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

# Platform specific configuration
if(WIN32)
    add_definitions(-DANTOMIC_PLATFORM_WINDOWS)
elseif (UNIX)
    add_definitions(-DANTOMIC_PLATFORM_LINUX)
endif(WIN32)

# Specify build paths
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/dist")

# POSIX Threads
find_package(Threads REQUIRED)

# SDL - Simple DirectMedia Layer
# https://www.libsdl.org/
if(NOT DEFINED ENV{ANTOMIC_NO_SDL})
    set(SDL_SUPPORT true)
    find_package(SDL2 REQUIRED)
    add_definitions(-DANTOMIC_SDL_PLATFORM)
    if(WIN32)
        add_definitions(-DANTOMIC_WINDOWS_SDL_PLATFORM)
    elseif(UNIX)
        add_definitions(-DANTOMIC_LINUX_SDL_PLATFORM)
    endif()
else()
    set(SDL_SUPPORT false)
    set(GLFW_SUPPORT true)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)    
    include(3rdparty/CMakeLists.glfw.txt)
    add_definitions(-DANTOMIC_GLFW_PLATFORM)
    if(WIN32)
        add_definitions(-DANTOMIC_WINDOWS_GLFW_PLATFORM)
    elseif(UNIX)
        add_definitions(-DANTOMIC_LINUX_GLFW_PLATFORM)
    endif()
endif() 

# std::chrono support
# https://en.cppreference.com/w/cpp/header/chrono
if(NOT DEFINED ENV{ANTOMIC_NO_CHRONO} AND NOT SDL_SUPPORT)
    add_definitions(-DANTOMIC_CHRONO_SUPPORT)
endif() 

# OpenGL RenderAPI Support
# https://www.opengl.org/
if(NOT DEFINED ENV{ANTOMIC_NO_OPENGL})
    add_definitions(-DANTOMIC_GL_RENDERER)
    if (UNIX)
        set("OpenGL_GL_PREFERENCE" "GLVND")
    endif(UNIX)
    find_package(OpenGL REQUIRED)
endif() 

# JSON Library setup
# https://github.com/nlohmann/json.git
include(3rdparty/CMakeLists.json.txt)

# Logging Library setup
# https://github.com/gabime/spdlog.git
include(3rdparty/CMakeLists.spdlog.txt)

# ImGui Library Setup
# https://github.com/ocornut/imgui.git
include(3rdparty/CMakeLists.imgui.txt)

# GLM Library setup
# https://github.com/g-truc/glm
include(3rdparty/CMakeLists.glm.txt)

# GLAD Multi-Language GL/GLES/EGL/GLX/WGL Loader-Generator based on the official specs.
# https://glad.dav1d.de/
add_subdirectory(3rdparty/glad)

# Antomic Engine
add_subdirectory(engine)
add_subdirectory(editor)
add_subdirectory(launcher)

# Unit Testing
# https://github.com/google/googletest.git
if(NOT DEFINED ENV{ANTOMIC_NO_TESTS})
    include(3rdparty/CMakeLists.googletest.txt)
    add_subdirectory(tests)
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

