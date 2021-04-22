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
# BUILD_LEVEL 1 - Run cmake generator
# BUILD_LEVEL 2 - Build the engine
# By default we install build dependencies, 3rd party dependencies and run the
# cmake generator
# Set the BUILD_LEVEL to the level you want to run 
$BUILD_LEVEL = if ($env:BUILD_LEVEL) { $env:BUILD_LEVEL } else { 1 };

# Set BUILD_LEVEL_ONLY flag to 1 to run only a specific BUILD_LEVEL 
$BUILD_LEVEL_ONLY = if ($env:BUILD_LEVEL_ONLY) { $env:BUILD_LEVEL_ONLY } else { 0 };

# Set the BUILD_TYPE, by default we build Debug
$BUILD_TYPE = if ($env:BUILD_TYPE) { $env:BUILD_TYPE } else { "Debug" };

$NINJA_URL = "https://github.com/ninja-build/ninja/releases/download/v1.10.2/ninja-win.zip"

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

    if (-not(Test-Path -Path "C:\msys64\Mingw64.exe" -PathType Leaf)) {
        Write-Host "To be able to install missing dependencies, please install MSYS2, for more information please visit https://www.msys2.org/"
        exit 1
    }
    Write-Host "Mingw64 found!"
    
    if (-not(Test-Path -Path "C:\msys64\mingw64\bin\g++.exe" -PathType Leaf)) {
        Write-Host "C++ compiler not found! Installing Compiler..."
        Start-Process -FilePath "C:\msys64\Mingw64.exe" -ArgumentList "./build.sh",$GENERATOR -Wait
    }
    
    Write-Host "C++ compiler Found!"
}

function setup_env_vars {

    $CURRENT_PATH = [System.Environment]::GetEnvironmentVariable("PATH")
    $USER_PATH = [System.Environment]::GetEnvironmentVariable("PATH", [System.EnvironmentVariableTarget]::User)
    
    $REQUIRED_PATH="C:\msys64\mingw64\bin"
    $CURRENT_PATH_FOUND = 0
    $USER_PATH_FOUND = 0
    foreach ($PATH_ITEM in $CURRENT_PATH.split(";")) {
        if ($PATH_ITEM -eq $REQUIRED_PATH) { $CURRENT_PATH_FOUND = 1 }
    }
    foreach ($PATH_ITEM in $USER_PATH.split(";")) {
        if ($PATH_ITEM -eq $REQUIRED_PATH) { $USER_PATH_FOUND = 1 }
    }
    if ($CURRENT_PATH_FOUND -eq 0) {
        [System.Environment]::SetEnvironmentVariable("PATH", $REQUIRED_PATH + ";" + $CURRENT_PATH)
    }
    if ($USER_PATH_FOUND -eq 0) {
        [System.Environment]::SetEnvironmentVariable("PATH", $REQUIRED_PATH + ";" + $USER_PATH, [System.EnvironmentVariableTarget]::User)
    }

    [System.Environment]::SetEnvironmentVariable("MINGW_CHOST", "x86_64-w64-mingw32")
    [System.Environment]::SetEnvironmentVariable("MINGW_PACKAGE_PREFIX", "mingw-w64-x86_64")
    [System.Environment]::SetEnvironmentVariable("MINGW_PREFIX", "/mingw64")
    [System.Environment]::SetEnvironmentVariable("MSYSCON", "mintty.exe")
    [System.Environment]::SetEnvironmentVariable("MSYSTEM", "MINGW64")
    [System.Environment]::SetEnvironmentVariable("MSYSTEM_CARCH", "x86_64")
    [System.Environment]::SetEnvironmentVariable("MSYSTEM_CHOST", "x86_64-w64-mingw32")
    [System.Environment]::SetEnvironmentVariable("MSYSTEM_PREFIX", "/mingw64")    

    [System.Environment]::SetEnvironmentVariable("MINGW_CHOST", "x86_64-w64-mingw32", [System.EnvironmentVariableTarget]::User)
    [System.Environment]::SetEnvironmentVariable("MINGW_PACKAGE_PREFIX", "mingw-w64-x86_64", [System.EnvironmentVariableTarget]::User)
    [System.Environment]::SetEnvironmentVariable("MINGW_PREFIX", "/mingw64", [System.EnvironmentVariableTarget]::User)
    [System.Environment]::SetEnvironmentVariable("MSYSCON", "mintty.exe", [System.EnvironmentVariableTarget]::User)
    [System.Environment]::SetEnvironmentVariable("MSYSTEM", "MINGW64", [System.EnvironmentVariableTarget]::User)
    [System.Environment]::SetEnvironmentVariable("MSYSTEM_CARCH", "x86_64", [System.EnvironmentVariableTarget]::User)
    [System.Environment]::SetEnvironmentVariable("MSYSTEM_CHOST", "x86_64-w64-mingw32", [System.EnvironmentVariableTarget]::User)
    [System.Environment]::SetEnvironmentVariable("MSYSTEM_PREFIX", "/mingw64", [System.EnvironmentVariableTarget]::User)    
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
    $CURRENT_PATH = $(Get-Location)

    cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=${BUILD_TYPE} -H"$CURRENT_PATH" -B"$CURRENT_PATH\build" -G "${CMAKE_GEN}"
}

function level_build_engine {
    Write-Host "level_build_engine"
}

# BUILD_LEVEL 0 - We just install the build dependencies
if ($BUILD_LEVEL -ge 0) {
    level_build_dependencies
}

setup_env_vars

# BUILD_LEVEL 1 - Run cmake generator
if ($BUILD_LEVEL -ge 1) {
    level_cmake_generator
}

# BUILD_LEVEL 1 - Build the engine
if ($BUILD_LEVEL -ge 2) {
    level_build_engine
}

Write-Host "All ok"
exit 0