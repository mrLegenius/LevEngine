#include "pch.h"
#include "RasterizerState.h"

#include "RenderSettings.h"
#include "Platform/D3D11/D3D11RasterizerState.h"

namespace LevEngine
{
	Ref<RasterizerState> RasterizerState::Create()
	{
		switch (RenderSettings::RendererAPI)
		{
		case RendererAPI::None:
			LEV_THROW("None for API was chosen")
		case RendererAPI::OpenGL:
			LEV_NOT_IMPLEMENTED
		case RendererAPI::D3D11:
			return CreateRef<D3D11RasterizerState>();
		default:
			LEV_THROW("Unknown Renderer API")
			break;
		}
	}
}
