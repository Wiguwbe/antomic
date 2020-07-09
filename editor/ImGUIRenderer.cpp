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
#include "ImGUIRenderer.h"
#include "Shaders/fs_ocornut_imgui.bin.h"
#include "Shaders/vs_ocornut_imgui.bin.h"

static const bgfx::EmbeddedShader s_embeddedShaders[] = {
    BGFX_EMBEDDED_SHADER(vs_ocornut_imgui),
    BGFX_EMBEDDED_SHADER(fs_ocornut_imgui), BGFX_EMBEDDED_SHADER_END()};

namespace Editor
{
    uint8_t ImGUIRenderer::s_View = 255;
    bgfx::ProgramHandle ImGUIRenderer::s_ShaderHandle = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle ImGUIRenderer::s_AttribLocationTex = BGFX_INVALID_HANDLE;
    bgfx::VertexLayout ImGUIRenderer::s_VertexLayout;

    void ImGUIRenderer::RenderDrawLists(ImDrawData *draw_data)
    {
        // Avoid rendering when minimized, scale coordinates for retina displays
        // (screen coordinates != framebuffer coordinates)
        ImGuiIO &io = ImGui::GetIO();
        if (io.DisplaySize.x == 0 || io.DisplaySize.y == 0)
        {
            return;
        }

        draw_data->ScaleClipRects(io.DisplayFramebufferScale);

        // Setup render state: alpha-blending enabled, no face culling,
        // no depth testing, scissor enabled
        uint64_t state =
            BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA |
            BGFX_STATE_BLEND_FUNC(
                BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);

        const bgfx::Caps *caps = bgfx::getCaps();

        // Setup viewport, orthographic projection matrix
        float ortho[16];
        bx::mtxOrtho(
            ortho, 0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, 0.0f, 1000.0f,
            0.0f, caps->homogeneousDepth);
        bgfx::setViewTransform(s_View, NULL, ortho);
        bgfx::setViewRect(s_View, 0, 0, (uint16_t)io.DisplaySize.x, (uint16_t)io.DisplaySize.y);

        // Render command lists
        for (int n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList *cmd_list = draw_data->CmdLists[n];
            uint32_t idx_buffer_offset = 0;

            bgfx::TransientVertexBuffer tvb;
            bgfx::TransientIndexBuffer tib;

            uint32_t numVertices = (uint32_t)cmd_list->VtxBuffer.size();
            uint32_t numIndices = (uint32_t)cmd_list->IdxBuffer.size();

            if ((numVertices != bgfx::getAvailTransientVertexBuffer(
                                    numVertices, s_VertexLayout)) ||
                (numIndices != bgfx::getAvailTransientIndexBuffer(numIndices)))
            {
                // not enough space in transient buffer, quit drawing the rest...
                break;
            }

            bgfx::allocTransientVertexBuffer(&tvb, numVertices, s_VertexLayout);
            bgfx::allocTransientIndexBuffer(&tib, numIndices);

            ImDrawVert *verts = (ImDrawVert *)tvb.data;
            memcpy(
                verts, cmd_list->VtxBuffer.begin(),
                numVertices * sizeof(ImDrawVert));

            ImDrawIdx *indices = (ImDrawIdx *)tib.data;
            memcpy(
                indices, cmd_list->IdxBuffer.begin(),
                numIndices * sizeof(ImDrawIdx));

            for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
            {
                const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[cmd_i];

                if (pcmd->UserCallback)
                {
                    pcmd->UserCallback(cmd_list, pcmd);
                }
                else
                {
                    const uint16_t xx = (uint16_t)bx::max(pcmd->ClipRect.x, 0.0f);
                    const uint16_t yy = (uint16_t)bx::max(pcmd->ClipRect.y, 0.0f);
                    bgfx::setScissor(
                        xx, yy, (uint16_t)bx::min(pcmd->ClipRect.z, 65535.0f) - xx,
                        (uint16_t)bx::min(pcmd->ClipRect.w, 65535.0f) - yy);

                    bgfx::setState(state);
                    bgfx::TextureHandle texture = {
                        (uint16_t)((intptr_t)pcmd->TextureId & 0xffff)};
                    bgfx::setTexture(0, s_AttribLocationTex, texture);
                    bgfx::setVertexBuffer(0, &tvb, 0, numVertices);
                    bgfx::setIndexBuffer(&tib, idx_buffer_offset, pcmd->ElemCount);
                    bgfx::submit(s_View, s_ShaderHandle);
                }

                idx_buffer_offset += pcmd->ElemCount;
            }
        }
    }

    void ImGUIRenderer::InitializeBgfx(uint8_t view)
    {
        if (bgfx::isValid(ImGUIRenderer::s_ShaderHandle))
            return;

        s_View = view;

        bgfx::RendererType::Enum type = bgfx::getRendererType();
        s_ShaderHandle = bgfx::createProgram(
            bgfx::createEmbeddedShader(s_embeddedShaders, type, "vs_ocornut_imgui"),
            bgfx::createEmbeddedShader(s_embeddedShaders, type, "fs_ocornut_imgui"),
            true);

        s_VertexLayout.begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();

        s_AttribLocationTex =
            bgfx::createUniform("g_AttribLocationTex", bgfx::UniformType::Sampler);
    }

    void ImGUIRenderer::ShudownBgfx()
    {
        bgfx::destroy(s_AttribLocationTex);
        bgfx::destroy(s_ShaderHandle);
    }

    ImTextureID ImGUIRenderer::GetTextureId(uint16_t width, uint16_t height, unsigned char *pixels, bgfx::TextureHandle &handle)
    {

        if (!bgfx::isValid(handle))
        {
            // Upload texture to graphics system
            handle = bgfx::createTexture2D(
                (uint16_t)width, (uint16_t)height, false, 1, bgfx::TextureFormat::BGRA8,
                0, bgfx::copy(pixels, width * height * 4));
        }

        return (void *)(intptr_t)handle.idx;
    }

} // namespace Editor
