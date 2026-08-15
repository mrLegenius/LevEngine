#pragma once

#include "Kernel/Core.h"
#include "RenderCommands.h"
#include "RenderStatistics.h"
#include "Pipeline/IndexBuffer.h"

namespace LevEngine
{
    class Application;

    class LEV_API RenderCommand
    {
    public:
        static void DrawIndexed(const Ref<IndexBuffer>& indexBuffer)
        {
            RenderStatistics::CountDrawCall(indexBuffer ? indexBuffer->GetCount() / 3 : 0);
            s_RendererAPI->DrawIndexed(indexBuffer);
        }

        static void DrawIndexedInstanced(const Ref<IndexBuffer>& indexBuffer, const uint32_t instanceCount)
        {
            RenderStatistics::CountDrawCall(indexBuffer ? indexBuffer->GetCount() / 3 * instanceCount : 0);
            s_RendererAPI->DrawIndexedInstanced(indexBuffer, instanceCount);
        }

        static void DrawLineList(const Ref<IndexBuffer>& indexBuffer)
        {
            RenderStatistics::CountDrawCall(0);
            s_RendererAPI->DrawLineList(indexBuffer);
        }

        static void DrawLineStrip(const uint32_t verticesCount)
        {
            RenderStatistics::CountDrawCall(0);
            s_RendererAPI->DrawLineStrip(verticesCount);
        }

        static void DrawFullScreenQuad()
        {
            RenderStatistics::CountDrawCall(2);
            s_RendererAPI->DrawFullScreenQuad();
        }

        static void DrawPointList(const uint32_t count)
        {
            RenderStatistics::CountDrawCall(0);
            s_RendererAPI->DrawPointList(count);
        }

    private:
        friend Application;

        static void Init() { s_RendererAPI = RenderCommands::Create(); }
        //Has to be called while the render device is still alive, see TextureLibrary::Shutdown
        static void Shutdown() { s_RendererAPI.reset(); }


        static inline Ref<RenderCommands> s_RendererAPI{};
    };
}
