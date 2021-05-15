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
#include <iostream>

namespace Antomic
{
	Game::Game() : Application("Game")
	{
	}

	void Game::LoadMainScene(const std::string& filePath)
	{
		if (!std::filesystem::exists(filePath))
		{
			ANTOMIC_ERROR("Scene {0}, does not exits");
			return;
		}

		// Load the scene
		std::ifstream sceneFile(filePath);
		nlohmann::json sceneJSON;
		sceneFile>> sceneJSON;

		auto scene = Scene::Deserialize(sceneJSON["scene"]);
		SetScene(scene);
	}
} // namespace Antomic
