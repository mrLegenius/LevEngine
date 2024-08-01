﻿#include "levpch.h"
#include "ConstantBuffer.h"

#include "Platform/D3D11/D3D11ConstantBuffer.h"
#include "Renderer/RenderSettings.h"

namespace LevEngine
{
Ref<ConstantBuffer> ConstantBuffer::Create(uint32_t size, uint32_t slot)
{
	switch (RenderSettings::RendererAPI)
	{
	case RendererAPI::None:
		LEV_CORE_ASSERT(false, "None for API was chosen");
	case RendererAPI::OpenGL:
		LEV_NOT_IMPLEMENTED
	case RendererAPI::D3D11:
		return CreateRef<D3D11ConstantBuffer>(size, slot);
	default:
		LEV_THROW("Unknown Renderer API");
	}
}
}
