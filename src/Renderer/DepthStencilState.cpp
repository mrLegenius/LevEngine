﻿#include "pch.h"
#include "DepthStencilState.h"

#include "RenderSettings.h"
#include "Platform/D3D11/D3D11DepthStencilState.h"

namespace LevEngine
{
	Ref<DepthStencilState> DepthStencilState::Create()
	{
		switch (RenderSettings::RendererAPI)
		{
		case RendererAPI::None:
			LEV_CORE_ASSERT(false, "None for API was chosen");
		case RendererAPI::OpenGL:
			LEV_NOT_IMPLEMENTED
		case RendererAPI::D3D11:
			return CreateRef<D3D11DepthStencilState>();
		default:
			LEV_THROW("Unknown Renderer API")
		}

		return nullptr;
	}
}