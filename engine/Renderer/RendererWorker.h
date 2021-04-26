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
#include "Core/Base.h"

namespace Antomic
{
    class RendererWorker
    {
    public:
        RendererWorker();
        ~RendererWorker();

    public:
        void Run();

    public:
        static void SubmitScene(const Ref<Scene> &scene);
        static const Ref<Scene> PopScene();
        static void StartWorker();
        static void StopWorker();

    private:
        bool mRunning;
        static QueueRef<Scene> sSceneQueue;
        static Ref<RendererWorker> sWorker;
        static std::mutex sMutex;
        static std::thread sThread;
    };

} // namespace Antomic
