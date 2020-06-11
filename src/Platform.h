#pragma once

class Game;

bool InitializePlatform(int width, int height, bool fullscreen);
void FinalizePlatform();
void RunPlatform(Game* game);