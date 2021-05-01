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
#include "gtest/gtest.h"
#include "Renderer/Buffers.h"
#include "Renderer/Shader.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace Antomic;

TEST(AntomicRendererTests, BufferLayoutTests)
{
    BufferLayout bLayout = {
        {ShaderDataType::Vec3, "aPos"},
        {ShaderDataType::Vec3, "aColor"},
        {ShaderDataType::Vec2, "aTexCoord"}};

    UniformBufferLayout uBLayout = {
        {ShaderDataType::Float, "value"},
        {ShaderDataType::Vec3, "vector"},
        {ShaderDataType::Mat4, "matrix"},
        {ShaderDataType::Float, 3, "values"},
        {ShaderDataType::Bool, "boolean"},
        {ShaderDataType::Int, "integer"}};

    EXPECT_EQ(bLayout.Stride(), 32);
    EXPECT_EQ(uBLayout.Stride(), 152);

    auto element = uBLayout.GetElement("vector");
    EXPECT_EQ(element.Name, "vector");
    EXPECT_EQ(element.BaseAlignment, ShaderDataTypeAlignment(ShaderDataType::Vec3));

    UniformBufferLayout uBLayout1 = {
        {ShaderDataType::Mat4, "m_proj"},
        {ShaderDataType::Mat4,  "m_view"},
        {ShaderDataType::Mat4, "m_projview"}};

    EXPECT_EQ(uBLayout1.Stride(), 192);
}
