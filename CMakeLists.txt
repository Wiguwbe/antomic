cmake_minimum_required(VERSION 3.5)

project(GameLaunch)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -Werror -std=c++17")

project(${PROJECT_NAME} CXX C)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_RELEASE 1)
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_RELEASE}")

if(WIN32)
    message(ERROR "Windows not supported yet")
endif(WIN32)

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(GL REQUIRED opengl)
pkg_check_modules(X11 REQUIRED x11)

set(VENDOR_DIR "${CMAKE_SOURCE_DIR}/vendor")

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

if (UNIX)
    set(BGFX_LIBRARY "${BGFX_DIR}/.build/linux64_gcc/bin/libbgfx-shared-libRelease.so")
endif(UNIX)

FILE(GLOB_RECURSE SRCS "${PROJECT_SOURCE_DIR}/src/*.cpp")
FILE(GLOB_RECURSE HDRS "${PROJECT_SOURCE_DIR}/src/*.h")

include_directories( 
    ${JSON_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIRS}
    ${GL_INCLUDE_DIRS}
    ${X11_INCLUDE_DIRS}
    ${BX_INCLUDE_DIRS}
    ${BGFX_INCLUDE_DIRS}
    ${BIMG_INCLUDE_DIRS}
)

# Specify build paths
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/dist")

add_executable(${PROJECT_NAME} ${HDRS} ${SRCS})
 
target_link_libraries(${PROJECT_NAME} 
                        ${BGFX_LIBRARY}
                        ${SDL2_LIBRARIES} 
                        ${GL_LIBRARIES} 
                        ${X11_LIBRARIES}
                        nlohmann_json::nlohmann_json
                        ${CMAKE_THREAD_LIBS_INIT}                       
                    )

file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX})
 
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
    CONFIGURATIONS Debug Release
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/media
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    CONFIGURATIONS Debug Release
)

install(FILES ${CMAKE_SOURCE_DIR}/settings.json
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    CONFIGURATIONS Debug Release
)


if(UNIX)
    install(FILES ${BGFX_LIBRARY}
        DESTINATION ${CMAKE_INSTALL_PREFIX}
        CONFIGURATIONS Debug Release
    )
endif(UNIX)
