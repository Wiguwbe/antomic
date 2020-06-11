/*
   Copyright 2020 Alexandre Pires (c.alexandre.pires@gmail.com)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include "Game.h"
#include "Log.h"
#include "Platform.h"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "nlohmann/json.hpp"

using namespace game;

Game::Game() : mWidth(640), mHeight(480), mFullscreen(false)
{
}

Game::~Game()
{
}

void Game::run()
{
    if (std::filesystem::exists("settings.json"))
    {
        std::ifstream settingsFile("settings.json");
        nlohmann::json settingsJSON;
        settingsFile >> settingsJSON;

        this->mWidth = settingsJSON["width"];
        this->mHeight = settingsJSON["height"];
        this->mFullscreen = settingsJSON["fullscreen"];
    }

    if (!platform::InitializePlatform(this->mWidth, this->mHeight, this->mFullscreen))
    {
        Logger.info("Error initializing platform");
        return;
    }

    Logger.info("Game initialized");
    platform::RunPlatform(this);
    platform::FinalizePlatform();
}

void Game::update()
{

    // Game update routine
}
