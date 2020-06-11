#!/bin/bash
# Install dependecies
sudo apt-get install libsdl2-2.0 libgl1-mesa-dev x11proto-core-dev libx11-dev

# Get external dependencies
mkdir -p vendor
pushd vendor
if [ ! -d bg ]; then
    git clone git://github.com/bkaradzic/bx.git
fi
if [ ! -d bgimg ]; then
    git clone git://github.com/bkaradzic/bimg.git
fi
if [ ! -d bgfx ]; then
    git clone git://github.com/bkaradzic/bgfx.git
    pushd bgfx
    make linux-release64
    popd
fi
if [ ! -d json ]; then
    git clone git://github.com/nlohmann/json.git
fi
popd
