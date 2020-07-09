#include "Core/LayerStack.h"

namespace Engine
{
    LayerStack::LayerStack()
    {
    }

    LayerStack::~LayerStack()
    {
    }

    void LayerStack::PushBack(Ref<Layer> l)
    {
        if (m_Stack.empty())
        {
            PushFront(l);
            return;
        }

        l->OnAttach();
        m_Stack.insert(m_Stack.begin(), l);
    }

    void LayerStack::PushFront(Ref<Layer> l)
    {
        l->OnAttach();
        m_Stack.push_back(l);
    }

    void LayerStack::PopBack()
    {
        if (m_Stack.empty())
            return;
        m_Stack.back()->OnDetach();
        m_Stack.pop_back();
    }

    void LayerStack::PopFront()
    {
        if (m_Stack.empty())
            return;
        m_Stack.back()->OnDetach();
        m_Stack.erase(m_Stack.begin());
    }

    void LayerStack::Remove(Ref<Layer> l)
    {
        auto it = std::find(m_Stack.begin(), m_Stack.end(), l);
        if (it != m_Stack.end())
        {
            (*it)->OnDetach();
            m_Stack.erase(it);
        }
    }

    void LayerStack::Update()
    {
        for (auto it = m_Stack.begin(); it != m_Stack.end(); ++it)
            (*it)->Update();
    }

    void LayerStack::OnEvent(Event &e)
    {
        for (auto it = m_Stack.end(); it != m_Stack.begin();)
        {
            (*--it)->OnEvent(e);
            if (e.IsHandled())
                break;
        }
    }

} // namespace Engine