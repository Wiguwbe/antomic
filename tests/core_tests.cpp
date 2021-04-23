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
#include "Events/Event.h"
#include "Events/WindowEvent.h"
#include "Core/Layer.h"
#include "Core/LayerStack.h"

static int testUpdate = 0;
static int testEvent = -1;

class TestLayer : public Antomic::Layer
{
private:
    /* data */
public:
    TestLayer() : Layer("test_layer"){};
    ~TestLayer(){};

    void OnEvent(Antomic::Event &e) override
    {
        e.SetHandled(true);
    }

    void Update(const Antomic::TimeStep& time) override
    {
        testUpdate = 1;
    }
};

class NumLayer : public Antomic::Layer
{
private:
    /* data */
public:
    NumLayer(int i, bool handle) : Layer("num_layer")
    {
        m_Num = i;
        m_Handle = handle;
    };
    ~NumLayer(){};

    void OnEvent(Antomic::Event &e) override
    {
        if (m_Handle)
        {
            testEvent = m_Num;
            e.SetHandled(true);
        }
    }

    void Update(const Antomic::TimeStep& time) override
    {
        testUpdate = m_Num;
    }

protected:
    bool m_Handle;
    int m_Num;
};

TEST(EngineCoreTest, LayerTests)
{
    Antomic::TimePoint now = Antomic::TimeClock::now();
    auto stack = Antomic::LayerStack();

    // A test layer
    auto layer = Antomic::CreateRef<TestLayer>();
    EXPECT_EQ(layer->GetName(), "test_layer");

    // We must have a layer in the stack
    stack.PushFront(layer);
    EXPECT_EQ(stack.Count(), 1);

    // The layer will handle the event
    Antomic::WindowCloseEvent e;
    EXPECT_EQ(e.IsHandled(), false);
    stack.OnEvent(e);
    EXPECT_EQ(e.IsHandled(), true);

    // The layer will run update
    EXPECT_EQ(testUpdate, 0);
    stack.Update(Duration(now-now));
    EXPECT_EQ(testUpdate, 1);

    // We will remove the layer, no layers left
    stack.PopFront();
    EXPECT_EQ(stack.Count(), 0);

    // We will add 10 layers, layers 4 or 1 will handle the event
    // Later we will remove layer #4, so we keep the reference to it
    Antomic::Ref<NumLayer> ref_layer = nullptr;
    for (auto i = 0; i < 10; i++)
    {
        auto l = Antomic::CreateRef<NumLayer>(i, i == 4 || i == 1);
        if (i == 4)
        {
            ref_layer = l;
        }
        stack.PushFront(l);
    }

    // We must have 10 layers in the stack
    EXPECT_EQ(stack.Count(), 10);

    // On this test, layer #4 will handle the event, testEvent == 4
    e.SetHandled(false);
    EXPECT_EQ(testEvent, -1);
    stack.OnEvent(e);
    EXPECT_EQ(testEvent, 4);

    // All events will update, so testUpdate will have the number 9
    testUpdate = 0;
    stack.Update(Duration(now-now));
    EXPECT_EQ(testUpdate, 9);

    // We remove the #4 layer and put it on the back
    stack.Remove(ref_layer);
    EXPECT_EQ(stack.Count(), 9);
    stack.PushBack(ref_layer);

    // Now #1 will handle the event
    e.SetHandled(false);
    stack.OnEvent(e);
    EXPECT_EQ(testEvent, 1);

    // We now remove all layers
    for (auto i = 0; i < 10; i++)
    {
        stack.PopFront();
    }
    EXPECT_EQ(stack.Count(), 0);
}