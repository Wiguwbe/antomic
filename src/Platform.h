#pragma once
#include "Game.h"

namespace platform
{
    bool InitializePlatform(int width, int height, bool fullscreen);
    void FinalizePlatform();
    void RunPlatform(game::Game *game);
} // namespace game
