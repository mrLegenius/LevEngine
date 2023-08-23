#include "pch.h"
#include "SamplerState.h"

#include "RenderSettings.h"
#include "Platform/D3D11/D3D11SamplerState.h"

namespace LevEngine
{
	Ref<SamplerState> SamplerState::Create()
	{
		switch (RenderSettings::RendererAPI)
		{
		case RendererAPI::None:
			LEV_CORE_ASSERT(false, "None for API was chosen");
		case RendererAPI::OpenGL:
			LEV_NOT_IMPLEMENTED
		case RendererAPI::D3D11:
			return CreateRef<D3D11SamplerState>();
		default:
			LEV_THROW("Unknown Renderer API")
			break;
		}
	}
}
