#include "pch.h"
#include "BlendState.h"

#include "RenderSettings.h"
#include "Platform/D3D11/D3D11BlendState.h"

namespace LevEngine
{
	BlendMode BlendMode::AlphaBlending = BlendMode{ true, false, BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendOperation::Add, BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha };
	BlendMode BlendMode::Additive = BlendMode{ true, false, BlendFactor::One, BlendFactor::One };

	Ref<BlendState> BlendState::Create()
	{
		switch (RenderSettings::RendererAPI)
		{
		case RendererAPI::None:
			LEV_CORE_ASSERT(false, "None for API was chosen");
		case RendererAPI::OpenGL:
			LEV_NOT_IMPLEMENTED
		case RendererAPI::D3D11:
			return CreateRef<D3D11BlendState>();
		default:
			LEV_THROW("Unknown Renderer API")
		}
	}
}
