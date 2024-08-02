#pragma once

#include "Platform/D3D11/D3D11RenderDevice.h"
#include "Renderer/RenderSettings.h"

namespace LevEngine
{
    inline Ref<RenderDevice> CreateRenderDevice()
    {
        switch (RenderSettings::RendererAPI)
        {
        case RendererAPI::None:
            LEV_CORE_ASSERT(false, "None for API was chosen");
            return nullptr;
        case RendererAPI::OpenGL:
            LEV_NOT_IMPLEMENTED
        case RendererAPI::D3D11:
            return CreateRef<D3D11RenderDevice>();
        default:
            LEV_THROW("Unknown Renderer API");
        }

        return nullptr;
    }

}