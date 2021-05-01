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
#include "Renderer/RendererFrame.h"
#include "Renderer/Renderer.h"
#include "Renderer/Drawable.h"
#include "RenderCommand.h"
#include "Core/Log.h"
#include "Profiling/Instrumentor.h"

namespace Antomic
{
    void RendererFrame::QueueDrawable(const Ref<Drawable> &drawable)
    {
        ANTOMIC_PROFILE_FUNCTION("Renderer");

        switch (drawable->GetType())
        {
        case DrawableType::SPRITE:
            mSpriteQueue.push(drawable);
            return;
        case DrawableType::MESH:
            mMeshQueue.push(drawable);
            return;
        default:
            ANTOMIC_ASSERT(false,"RendererFrame::QueueDrawable: Type not handled");
            return;
        }
    }

    void RendererFrame::Draw()
    {

        ANTOMIC_PROFILE_FUNCTION("Renderer");

        // Start the rendering process
        RenderCommand::SetViewport(mViewport.Left, mViewport.Top, mViewport.Right, mViewport.Bottom);
        RenderCommand::SetClearColor(mViewport.Color);
        RenderCommand::Clear();

        // First we draw the 3D elements
        while (!mMeshQueue.empty())
        {
            auto drawable = mMeshQueue.front();
            mMeshQueue.pop();
            drawable->Draw();
        }

        // Now we draw the 2D elements
        while (!mSpriteQueue.empty())
        {
            auto drawable = mSpriteQueue.front();
            mSpriteQueue.pop();
            drawable->Draw();
        }
    }

} // namespace Antomic
