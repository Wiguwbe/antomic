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
#include "Core/Base.h"
#include "Core/Log.h"
#include "TestNode2d.h"
#include "glm/glm.hpp"
#include <glm/gtx/matrix_transform_2d.hpp>

using namespace Antomic;

TEST(AntomicGraphTest, Node2dTests)
{
    auto root = CreateRef<TestNode2d>();
    EXPECT_EQ(root->GetParent(), nullptr);
    EXPECT_EQ(root->GetChildren().size(), 0);
    EXPECT_EQ(root->GetWorldMatrix(), glm::mat3(1.0f));
    EXPECT_EQ(root->GetWorldMatrix(), root->GetLocalMatrix());

    auto child1 = CreateRef<TestNode2d>();
    EXPECT_EQ(child1->GetParent(), nullptr);
    EXPECT_EQ(child1->GetWorldMatrix(), glm::mat3(1.0f));
    EXPECT_EQ(child1->GetWorldMatrix(), child1->GetLocalMatrix());

    root->AddChild(child1);
    EXPECT_EQ(child1->GetParent(), root);
    EXPECT_EQ(root->GetChildren().size(), 1);

    auto child2 = CreateRef<TestNode2d>();
    EXPECT_EQ(child2->GetParent(), nullptr);
    EXPECT_EQ(child2->GetWorldMatrix(), glm::mat3(1.0f));
    EXPECT_EQ(child2->GetWorldMatrix(), child2->GetLocalMatrix());

    root->AddChild(child2);
    EXPECT_EQ(child2->GetParent(), root);
    EXPECT_EQ(root->GetChildren().size(), 2);

    root->RemoveChild(child1);
    EXPECT_EQ(root->GetChildren().size(), 1);
    EXPECT_EQ(child1->GetParent(), nullptr);

    root->RemoveChild(child2);
    EXPECT_EQ(root->GetChildren().size(), 0);
    EXPECT_EQ(child2->GetParent(), nullptr);

    root->AddChild(child1);
    child1->AddChild(child2);

    auto t1 = glm::translate(glm::mat3(1.0f), glm::vec2(-1.0f, 0.0f));
    auto m1 = glm::rotate(t1, glm::radians(45.f));

    child1->SetLocalMatrix(m1);
    EXPECT_EQ(child1->GetLocalMatrix(), m1);
    EXPECT_EQ(child2->GetWorldMatrix(), m1);

    auto t2 = glm::translate(glm::mat3(1.0f), glm::vec2(-1.0f, -1.0f));
    auto m2 = glm::rotate(t2, glm::radians(45.f));

    child2->SetLocalMatrix(m2);
    EXPECT_EQ(child2->GetLocalMatrix(), m2);
    EXPECT_EQ(child2->GetWorldMatrix(), m1 * m2);
}
