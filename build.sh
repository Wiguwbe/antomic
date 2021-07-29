#!/bin/bash
#    Copyright 2020 Alexandre Pires (c.alexandre.pires@gmail.com)
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
set +e
printf "Antomic Engine project setup - Bash Version\n"

# This BUILD_LEVEL is used to understand what we are setting up:
# When you set a level number all the below levels will be installed also
# BUILD_LEVEL 0 - We just install the build dependencies
# BUILD_LEVEL 1 - Run cmake generator
# BUILD_LEVEL 2 - Build the engine
# By default we install build dependencies, 3rd party dependencies and run the
# cmake generator
# Set the BUILD_LEVEL to the level you want to run 
[ -z ${BUILD_LEVEL} ] && BUILD_LEVEL=1

# Set BUILD_LEVEL_ONLY flag to 1 to run only a specific BUILD_LEVEL 
[ -z ${BUILD_LEVEL_ONLY} ] && BUILD_LEVEL_ONLY=0

# Set the BUILD_TYPE, by default we build Debug
[ -z ${BUILD_TYPE} ] && BUILD_TYPE="Debug"

# Select generatior
if [ $# -eq 0 ]; then
    printf "Usage:\n"
    printf "build.sh <environment>\n"
    printf "available environments:\n"
    printf "* Ninja\n"
    printf "* Kate\n"
    printf "* Codelite\n"
    exit 0
fi

function level_build_dependencies() {
    
    # If we are enabled the BUILD_LEVEL_ONLY_FLAG and this is not
    # the level supposed to build, just skip it
    [ ${BUILD_LEVEL_ONLY} -eq 1 ] && [ ${BUILD_LEVEL} -ne 0 ] && return
    printf "Installing Build Dependencies\n"

    case "${DISTRO}" in 
        Ubuntu)
            echo "System: Ubuntu - Gcc"
            PACKAGES=(
                "build-essential"
                "cmake"
                "cmake-extras"
                "ninja-build"
                "libglew-dev"
                "libsdl2-dev"
                "libgl1-mesa-dev"
                "x11proto-core-dev"
                "libx11-dev"
                "mesa-common-dev"
            )

            MISSING_PACKAGES=""
            for PACKAGE in ${PACKAGES[@]}; do
                dpkg --get-selections | grep -q ${PACKAGE} || MISSING_PACKAGES+="${PACKAGE} "
            done

            # This might not be ok, but for now if we only have this missing,
            # since is a meta-package my be a false positive
            [ "${MISSING_PACKAGES}" == "build-essentials" ] && MISSING_PACKAGES=""

            # If there are any packages to install compose the installation command
            if [ ! -z "$MISSING_PACKAGES" ]; then 
                sudo apt update;
                sudo apt-get -y install ${MISSING_PACKAGES};
            fi
            ;;
        Clear*)
            echo "System: Clear Linux - Gcc"
            # On Clear Linux for now we install games-dev bundle
            # probably in the future we need to look back into this
            sudo swupd bundle-add games-dev
            ;;
        MINGW64)
            echo "System: Windows - MingGW 54"
            PACKAGES=(
                "mingw-w64-x86_64-toolchain"
                "mingw-w64-x86_64-cmake"
                "mingw-w64-x86_64-extra-cmake-modules"
                "mingw-w64-x86_64-cninja"
                "mingw-w64-x86_64-gdb"
                "mingw-w64-x86_64-SDL2"
                "mingw-w64-x86_64-glew"
            )

            MISSING_PACKAGES=""
            for PACKAGE in ${PACKAGES[@]}; do
                MISSING_PACKAGES+="${PACKAGE} "
            done

            pacman -S ${MISSING_PACKAGES};
            ;;
        *)
            printf "Distro not supported yet (%s)\n" ${DISTRO}
            exit 0;;
    esac

}

function level_cmake_generator() {
    
    # If we are enabled the BUILD_LEVEL_ONLY_FLAG and this is not
    # the level supposed to build, just skip it
    [ ${BUILD_LEVEL_ONLY} -eq 1 ] && [ ${BUILD_LEVEL} -ne 2 ] && return
    printf "Generate CMake Project\n"
    CMAKE_CMD=$(command -v cmake)
    if [ -z $CMAKE_CMD ]; then
        printf "CMake is missing, something is wrong!\n"
        exit 1
    fi
    mkdir -p build
    ${CMAKE_CMD} --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=${BUILD_TYPE} -H"$(pwd)" -B"$(pwd)/build" -G "${CMAKE_GEN}"
}

function level_build_engine() {
    
    # If we are enabled the BUILD_LEVEL_ONLY_FLAG and this is not
    # the level supposed to build, just skip it
    [ ${BUILD_LEVEL_ONLY} -eq 1 ] && [ ${BUILD_LEVEL} -ne 0 ] && return
    
    # We must have the build folder
    [ -d build ] || { printf "Build folder missing.\n"; exit 1; }

    cmake --build $(pwd)/build
}

# What cmake generator we will be using
case "${1}" in
    [Cc]odelite)
        CMAKE_GEN="CodeLite - Unix Makefiles";;
    [Kk]ate)
        CMAKE_GEN="Kate - Ninja";;
    [Nn]inja)
        CMAKE_GEN="Ninja";;
    *)
        printf "Unknown generator %s" ${CMAKE_GEN}
        exit 1;;
esac

# Check if we are building on Linux or OSX
OSUNAME="$(uname -s)"
case "${OSUNAME}" in
    Linux*)
        OS="Linux"
        unset INSTALL_PACKAGES_CMD
        if [ -f /etc/os-release ]; then
            # freedesktop.org and systemd
            . /etc/os-release
            DISTRO=$NAME
        fi
        ;;
    Darwin*)    
        OS="OSX"
        printf "MacOS not supported yet."
        exit 0;;
    MINGW64*)
        OS="Windows"
        DISTRO="MINGW64";;
    *)          
        printf "Unknown OS"
        exit 0;;
esac

# BUILD_LEVEL 0 - We just install the build dependencies
[ ${BUILD_LEVEL} -ge 0 ] && level_build_dependencies

if [ ${OS} == "Windows" ]; then 
    printf "For windows we just install dependencies!\n"
    exit 0
fi

# BUILD_LEVEL 1 - Run cmake generator
[ ${BUILD_LEVEL} -ge 1 ] && level_cmake_generator

# BUILD_LEVEL 1 - Build the engine
[ ${BUILD_LEVEL} -ge 2 ] && level_build_engine

printf "All ok\n"
exit 0