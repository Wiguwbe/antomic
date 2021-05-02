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
#include "Core/Base.h"
#include "TestNode3d.h"
#include "glm/glm.hpp"

using namespace Antomic;

void TestNode3d::UpdateSpatialInformation()
{
    if (!IsDirty())
    {
        return;
    }

    // Update the world matrix
    auto parent = std::dynamic_pointer_cast<Node3d>(GetParent());
    mWorld = (parent == nullptr) ? mLocal : parent->GetWorldMatrix() * mLocal;

    ClearDirty();
}
