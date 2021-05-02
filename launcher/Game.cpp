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

namespace Antomic
{
    Game::Game() : Application("Game")
    {
    }

    void Game::LoadMainScene(const std::string &scene)
    {
        {
            // Create a new scene
            auto scene = CreateRef<Scene>();
            // Load a new sprite
            auto sprite = CreateRef<SpriteNode>("assets/textures/container.jpg");
            // Add sprite to the scene
            sprite->SetPosition(glm::vec2(0, 0));
            sprite->SetSize(glm::vec2(50.0f, 50.0f));
            sprite->SetRotation(45.f);
            sprite->SetColor({0.0f, 1.0f, 0.0f,1.0f});
            scene->AddChild(sprite);
            SetScene(scene);
        }
    }
} // namespace Antomic
