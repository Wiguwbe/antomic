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
#include "Core/Serialization.h"
#include "nlohmann/json.hpp"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

TEST(AntomicCoreTest, SerializationTests)
{
    // A test layer
    // auto layer = Antomic::CreateRef<TestLayer>();
    // EXPECT_EQ(layer->GetName(), "test_layer");

    // TODO: Check if there is a way to take advantage of the mWorld matrix
    // auto model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(mPosition, 0.0f));
    // model = glm::translate(model, glm::vec3(0.5f * mSize.x, 0.5f * mSize.y, 0.0f));
    // model = glm::rotate(model, glm::radians(mRotation), glm::vec3(0.0f, 0.0f, 1.0f));
    // model = glm::translate(model, glm::vec3(-0.5f * mSize.x, -0.5f * mSize.y, 0.0f));
    // model = glm::scale(model, glm::vec3(mSize, 1.0f));

    nlohmann::json j;

    // We must have a layer in the stack
    glm::vec2 in_v1(1.0f, 0.f);
    glm::vec2 out_v1;
    Antomic::Serialization::Serialize(j["v1"], in_v1);
    Antomic::Serialization::Deserialize(j["v1"], out_v1);
    EXPECT_EQ(in_v1, out_v1);

    glm::vec3 in_v2(1.f, 1.f, 0.9f);
    glm::vec3 out_v2;
    Antomic::Serialization::Serialize(j["v2"], in_v2);
    Antomic::Serialization::Deserialize(j["v2"], out_v2);
    EXPECT_EQ(in_v2, out_v2);

    glm::vec4 in_v3(1.f, 1.f, 0.9f, 0.5f);
    glm::vec4 out_v3;
    Antomic::Serialization::Serialize(j["v3"], in_v3);
    Antomic::Serialization::Deserialize(j["v3"], out_v3);
    EXPECT_EQ(in_v3, out_v3);

    glm::mat4 in_m1 = glm::rotate(glm::mat4(1.0f), glm::radians(45.f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 out_m1;
    Antomic::Serialization::Serialize(j["m1"], in_m1);
    Antomic::Serialization::Deserialize(j["m1"], out_m1);
    EXPECT_EQ(in_m1, out_m1);

    glm::mat3 in_m2 = glm::rotate(glm::mat3(1.0f), glm::radians(15.f));
    glm::mat3 out_m2;
    Antomic::Serialization::Serialize(j["m2"], in_m2);
    Antomic::Serialization::Deserialize(j["m2"], out_m2);
    EXPECT_EQ(in_m2, out_m2);
}