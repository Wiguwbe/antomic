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
#ifdef ANTOMIC_TESTS
#pragma once
#include "Core/Base.h"
#include "Graph/3D/Node3d.h"
#include "glm/glm.hpp"

namespace Antomic
{
    class TestNode3d : public Node3d
    {
    public:
        TestNode3d() = default;
        virtual ~TestNode3d() = default;

    public:
        // Spatial information
        inline void SetLocalMatrix(const glm::mat4 &matrix)
        {
            mLocal = matrix;
            MakeDirty();
        }

    protected:
        virtual const Ref<Drawable> GetDrawable() const override { return nullptr; };
        virtual void UpdateSpatialInformation() override;            
    };
} // namespace Antomic
#endif