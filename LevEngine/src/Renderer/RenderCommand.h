#pragma once
#include "RenderCommands.h"

namespace LevEngine
{
    class Application;
    
    class RenderCommand
    {
    public:
        static void DrawIndexed(const Ref<IndexBuffer>& indexBuffer)
        {
            s_RendererAPI->DrawIndexed(indexBuffer);
        }

        static void DrawLineList(const Ref<IndexBuffer>& indexBuffer)
        {
            s_RendererAPI->DrawLineList(indexBuffer);
        }

        static void DrawLineStrip(const uint32_t verticesCount)
        {
            s_RendererAPI->DrawLineStrip(verticesCount);
        }

        static void DrawFullScreenQuad()
        {
            s_RendererAPI->DrawFullScreenQuad();
        }

        static void DrawPointList(const uint32_t count)
        {
            s_RendererAPI->DrawPointList(count);
        }

    private:
        friend Application;

        static void Init() { s_RendererAPI = RenderCommands::Create(); }
        
        static inline Ref<RenderCommands> s_RendererAPI{};
    };
}
