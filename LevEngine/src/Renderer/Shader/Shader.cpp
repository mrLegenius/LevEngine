#include "levpch.h"
#include "Shader.h"

#include "Kernel/Application.h"
#include "Renderer/RenderDevice.h"

namespace LevEngine
{
	Ref<Shader> Shader::Create(const String& filepath, ShaderType shaderTypes)
	{
		return App::RenderDevice().CreateShader(filepath, shaderTypes, ShaderMacros{});
	}
	
	Ref<Shader> Shader::Create(const String& filepath, ShaderType shaderTypes, const ShaderMacros& macros)
	{
		return App::RenderDevice().CreateShader(filepath, shaderTypes, macros);
	}

	Ref<Shader> Shader::Create(const String& filepath)
	{
		return Create(filepath, ShaderType::Pixel | ShaderType::Vertex);
	}
}
