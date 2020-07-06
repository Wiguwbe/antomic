#include "Core/LayerStack.h"

namespace Engine
{
    LayerStack::LayerStack()
    {
    }

    LayerStack::~LayerStack()
    {
    }

    void LayerStack::Push(Ref<Layer> l)
    {
        l->OnAttach();
        m_Layers.push_back(l);
    }

    void LayerStack::Pop()
    {
        if (m_Layers.empty())
            return;
        m_Layers.back()->OnDettach();
        m_Layers.pop_back();
    }

    void LayerStack::Remove(Ref<Layer> l)
    {
        auto it = std::find(m_Layers.begin(), m_Layers.end(), l);
        if (it != m_Layers.end())
        {
            (*it)->OnDettach();
            m_Layers.erase(it);
        }
    }

    void LayerStack::ForwardUpdate()
    {
        for (auto it = m_Layers.begin(); it != m_Layers.end(); ++it)
        {

            (*it)->Update();
        }
    }

    void LayerStack::ReverseUpdate()
    {
        for (auto it = m_Layers.end(); it != m_Layers.begin(); --it)
        {
            (*it)->Update();
        }
    }

    void LayerStack::ForwardOnEvent(Event &e)
    {
        for (auto it = m_Layers.begin(); it != m_Layers.end(); ++it)
        {
            (*it)->HandleEvent(e);
            if (e.IsHandled())
                break;
        }
    }

    void LayerStack::ReverseOnEvent(Event &e)
    {
        for (auto it = m_Layers.end(); it != m_Layers.begin(); --it)
        {
            (*it)->HandleEvent(e);
            if (e.IsHandled())
                break;
        }
    }

} // namespace Engine