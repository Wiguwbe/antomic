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

Param(
    [string]$GENERATOR = $null
)

Write-Host "Antomic Engine project setup - Powershell Version";

# This BUILD_LEVEL is used to understand what we are setting up:
# When you set a level number all the below levels will be installed also
# BUILD_LEVEL 0 - We just install the build dependencies
# BUILD_LEVEL 1 - We install 3rd party dependencies
# BUILD_LEVEL 2 - Run cmake generator
# BUILD_LEVEL 3 - Build the engine
# By default we install build dependencies, 3rd party dependencies and run the
# cmake generator
# Set the BUILD_LEVEL to the level you want to run 
$BUILD_LEVEL = if ($env:BUILD_LEVEL) { $env:BUILD_LEVEL } else { 2 };

# Set BUILD_LEVEL_ONLY flag to 1 to run only a specific BUILD_LEVEL 
$BUILD_LEVEL_ONLY = if ($env:BUILD_LEVEL_ONLY) { $env:BUILD_LEVEL_ONLY } else { 0 };

# Set the BUILD_TYPE, by default we build Debug
$BUILD_TYPE = if ($env:BUILD_TYPE) { $env:BUILD_TYPE } else { "Debug" };

switch -Regex ($GENERATOR) {
    '^[Nn]inja$' { $CMAKE_GEN = "Ninja" }
    '^[Vv][Ss]2017$' { $CMAKE_GEN = "Visual Studio 15 2017" }
    '^[Vv][Ss]2019$' { $CMAKE_GEN = "Visual Studio 16 2019" }
    Default {
        Write-Host "Usage:";
        Write-Host "build.ps1 <environment>";
        Write-Host "available environments:";
        Write-Host "* Ninja";
        Write-Host "* VS2017";
        Write-Host "* VS2019";   
        exit 0
    }
}

function level_build_dependencies {

    # If we are enabled the BUILD_LEVEL_ONLY_FLAG and this is not
    # the level supposed to build, just skip it    
    if ( $BUILD_LEVEL_ONLY -eq 1 -And $BUILD_LEVEL -ne 0 ) {
        return
    }
    Write-Host "Installing Build Dependencies"

    # Firt check if we have the tools we need
    if (Get-Command winget.exe) {
        Write-Host "Winget found!"
    }
    else {
        Write-Host "To be able to install missing dependencies, please install winget.exe, for more information please visit https://docs.microsoft.com/en-us/windows/package-manager/winget/"
        exit 1
    }

    if (Get-Command "C:\Program Files\CMake\bin\cmake.exe") {
        Write-Host "CMake found!"
    }
    else {
        Write-Host "CMake not found! Installing CMake..."
        winget.exe install Kitware.CMake
    }

    if (Get-Command git.exe) {
        Write-Host "Git found!"
    }
    else {
        Write-Host "Git not found! Installing Git..."
        winget.exe install Git.Git
    }

    if (Get-Command "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\devenv.COM") {
        Write-Host "Visual Studio 2017 found!"
    }
    else { 
        Write-Host "Visual Studio 2017 not found! Please install Visual Studio 2017 with windows 8.1 SDK before continuing..."
        exit 0
    }
}

function level_3dparty_dependencies {
    # If we are enabled the BUILD_LEVEL_ONLY_FLAG and this is not
    # the level supposed to build, just skip it    
    if ( $BUILD_LEVEL_ONLY -eq 1 -And $BUILD_LEVEL -ne 1 ) {
        return
    }
    Write-Host "Installing 3rd Party Dependencies"
    $VENDORS_GIT = Get-Content .\dependencies.git

    # Create the vendor directory
    mkdir -p vendor 2>&1 | Out-Null

    foreach ($GIT in $VENDORS_GIT) {
        # We get the basename of the folder without the
        # extension
        $BASENAME = $GIT.split("/")[-1].split(".")[0]
        if (Test-Path "vendor/$BASENAME") {
            continue
        }
        git clone $GIT vendor/$BASENAME
        Write-Host "$GIT"
    }
    
    # Get SDL from the oficial website
    Invoke-WebRequest https://www.libsdl.org/release/SDL2-devel-2.0.14-VC.zip -OutFile $env:TEMP\SDL2-devel-2.0.14-VC.zip
    Expand-Archive -LiteralPath "$env:TEMP\SDL2-devel-2.0.14-VC.zip" -DestinationPath ./vendor

    # Build BGFX will be deprecated soon
    $Old_Path = $Env:Path 
    $Env:Path += ";C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE"
    
    # we make sure bgfx is built
    Push-Location vendor/bgfx
    ..\bx\tools\bin\windows\genie.exe --with-tools --with-combined-examples --with-shared-lib vs2017
    devenv .build/projects/vs2017/bgfx.sln /Build "Debug|x64"
    Pop-Location    

    $Env:Path = $Old_Path
}

function level_cmake_generator {
    # If we are enabled the BUILD_LEVEL_ONLY_FLAG and this is not
    # the level supposed to build, just skip it    
    if ( $BUILD_LEVEL_ONLY -eq 1 -And $BUILD_LEVEL -ne 2 ) {
        return
    }
    Write-Host "Generating CMake Project"

    # If we are enabled the BUILD_LEVEL_ONLY_FLAG and this is not
    # the level supposed to build, just skip it
    if (${BUILD_LEVEL_ONLY} -eq 1 -And ${BUILD_LEVEL} -ne 2 ) {
        return
    } 
    
    mkdir -p build 2>&1 | Out-Null
    $CURRENT_PATH=$(Get-Location)

    $Old_Path = $Env:Path 
    $Env:Path += ";C:\Program Files\CMake\bin"
    cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=${BUILD_TYPE} -H"$CURRENT_PATH" -B"$CURRENT_PATH\build" -G "${CMAKE_GEN}"
    $Env:Path = $Old_Path
}

function level_build_engine {
    Write-Host "level_build_engine"
}

# BUILD_LEVEL 0 - We just install the build dependencies
if ($BUILD_LEVEL -ge 0) {
    level_build_dependencies
}

# BUILD_LEVEL 1 - We install 3rd party dependencies
if ($BUILD_LEVEL -ge 1) {
    level_3dparty_dependencies
}

# BUILD_LEVEL 1 - Run cmake generator
if ($BUILD_LEVEL -ge 2) {
    level_cmake_generator
}

# BUILD_LEVEL 1 - Build the engine
if ($BUILD_LEVEL -ge 3) {
    level_build_engine
}

Write-Host "All ok"
exit 0