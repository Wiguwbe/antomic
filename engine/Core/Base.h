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
#pragma once
#include "enginepch.h"

#ifdef ANTOMIC_DEBUG
#if defined(ANTOMIC_PLATFORM_WINDOWS)
#define ANTOMIC_DEBUGBREAK() __debugbreak()
#elif defined(ANTOMIC_PLATFORM_LINUX)
#include <signal.h>
#define ANTOMIC_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif
#define ANTOMIC_ENABLE_ASSERTS
#else
#define ANTOMIC_DEBUGBREAK()
#endif

#ifdef ANTOMIC_ENABLE_ASSERTS
#define ANTOMIC_ASSERT(x, ...)                                   \
    {                                                           \
        if (!(x))                                               \
        {                                                       \
            ANTOMIC_TRACE("Assertion Failed: {0}", __VA_ARGS__); \
            ANTOMIC_DEBUGBREAK();                                \
        }                                                       \
    }
#else
#define ANTOMIC_ASSERT(x, y, ...)
#endif

#define BIT(x) 1 << x

#define ANTOMIC_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Antomic
{
    template <typename T>
    using Scope = std::unique_ptr<T>;

    template <typename T, typename... Args>
    constexpr Scope<T> CreateScope(Args &&... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    using Ref = std::shared_ptr<T>;

    template <typename T, typename... Args>
    constexpr Ref<T> CreateRef(Args &&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    class Application;
    class Platform;
    class Input;
    class Log;
    class Event;
    class EventListener;
    class WindowCloseEvent;
    class WindowResizeEvent;

    using EventHandler = std::function<void(Event&)>;

} // namespace Antomic

