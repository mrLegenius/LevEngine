﻿#include "levpch.h"
#include "Shader.h"

#include "Platform/D3D11/D3D11Shader.h"
#include "Renderer/RenderSettings.h"

namespace LevEngine
{
	Ref<Shader> Shader::Create(const String& filepath, ShaderType shaderTypes)
	{
		switch (RenderSettings::RendererAPI)
		{
		case RendererAPI::None:
			LEV_CORE_ASSERT(false, "None for API was chosen");
		case RendererAPI::OpenGL:
			LEV_NOT_IMPLEMENTED
		case RendererAPI::D3D11:
			return CreateRef<D3D11Shader>(filepath, shaderTypes);
		default:
			LEV_THROW("Unknown Renderer API")
			break;
		}
	}

	Ref<Shader> Shader::Create(const String& filepath)
	{
		switch (RenderSettings::RendererAPI)
		{
		case RendererAPI::None:
			LEV_CORE_ASSERT(false, "None for API was chosen");
		case RendererAPI::OpenGL:
			LEV_NOT_IMPLEMENTED
		case RendererAPI::D3D11:
			return CreateRef<D3D11Shader>(filepath);
		default:
			LEV_THROW("Unknown Renderer API")
			break;
		}
	}
}