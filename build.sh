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
# BUILD_LEVEL 1 - We install 3rd party dependencies
# BUILD_LEVEL 2 - Run cmake generator
# BUILD_LEVEL 3 - Build the engine
# By default we install build dependencies, 3rd party dependencies and run the
# cmake generator
# Set the BUILD_LEVEL to the level you want to run 
[ -z ${BUILD_LEVEL} ] && BUILD_LEVEL=2

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
            PACKAGES=(
                "build-essentials"
                "cmake"
                "cmake-extra"
                "libsdl2-dev"
                "libgl1-mesa-dev"
                "x11proto-core-dev"
                "libx11-dev"
                "mesa-common-dev"
            )

            MISSING_PACKAGES=()
            for PACKAGE in ${PACKAGES[@]}; do
                dpkg --get-selections | grep -q ${PACKAGE} || MISSING_PACKAGES+=${PACKAGE}
            done

            # This might not be ok, but for now if we only have this missing,
            # since is a meta-package my be a false positive
            [ "${MISSING_PACKAGES}" == "build-essentials" ] && MISSING_PACKAGES=()

            # If there are any packages to install compose the installation command
            [ ${#MISSING_PACKAGES[@]} -gt 0 ] && INSTALL_PACKAGES_CMD="sudo apt-get -y install ${MISSING_PACKAGE[@]}"
        ;;
        *)
            printf "Distro not supported yet (%s)\n" ${DISTRO}
            exit 0;;
    esac

    # Is the installation command not empty? run it
    [ ! -z "${INSTALL_PACKAGES_CMD}" ] && { 
        # This will most probably happend when setting up the environment for the first time
        # therefore we exit here to make sure the user run the build script again
        # this happens because we might being only setting up the build environment for 
        # automation ( ie. Docker image )
        printf "There are missing packages, will install them now!\n"; 
        ${INSTALL_PACKAGES_CMD}; 
    }
}

function level_3dparty_dependencies() {
    
    # If we are enabled the BUILD_LEVEL_ONLY_FLAG and this is not
    # the level supposed to build, just skip it
    [ ${BUILD_LEVEL_ONLY} -eq 1 ] && [ ${BUILD_LEVEL} -ne 1 ] && return
    printf "Installing 3rd Party dependencies\n"

    VENDORS_GIT=(
        "git://github.com/nlohmann/json.git"
        "git://github.com/gabime/spdlog.git"
        "git://github.com/ocornut/imgui.git"
    )

    # Create the vendor directory
    mkdir -p vendor

    # push all 3rd party repositories
    for GIT in ${VENDORS_GIT[@]}; do
        REPONAME=$(basename ${GIT})
        [ -d "vendor/${REPONAME%.git}" ] && continue
        git clone ${GIT} "vendor/${REPONAME%.git}"
    done

    # we make sure bgfx is built
    pushd vendor/bgfx
    make linux-release64
    popd
}

function level_cmake_generator() {
    
    # If we are enabled the BUILD_LEVEL_ONLY_FLAG and this is not
    # the level supposed to build, just skip it
    [ ${BUILD_LEVEL_ONLY} -eq 1 ] && [ ${BUILD_LEVEL} -ne 2 ] && return
    CMAKE_CMD=$(command -v cmake)
    mkdir -p build
    ${CMAKE_CMD} --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=${BUILD_TYPE} -H$(pwd) -B$(pwd)/build -G "${CMAKE_GEN}"
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
        OS=Linux;;
    Darwin*)    
        printf "MacOS not supported yet."
        exit 0;;
    *)          
        printf "Unknown OS"
        exit 0;;
esac

# Check in what distro we are working
if [ "${OS}" = "Linux" ]; then
 
    unset INSTALL_PACKAGES_CMD
    if [ -f /etc/os-release ]; then
        # freedesktop.org and systemd
        . /etc/os-release
        DISTRO=$NAME
    fi
fi

# BUILD_LEVEL 0 - We just install the build dependencies
[ ${BUILD_LEVEL} -ge 0 ] && level_build_dependencies

# BUILD_LEVEL 1 - We install 3rd party dependencies
[ ${BUILD_LEVEL} -ge 1 ] && level_3dparty_dependencies

# BUILD_LEVEL 1 - Run cmake generator
[ ${BUILD_LEVEL} -ge 2 ] && level_cmake_generator

# BUILD_LEVEL 1 - Build the engine
[ ${BUILD_LEVEL} -ge 3 ] && level_build_engine

printf "All ok\n"
exit 0