#include "levpch.h"
#include "Shader.h"

#include "Kernel/Application.h"
#include "Renderer/RenderDevice.h"

namespace LevEngine
{
	Ref<Shader> Shader::Create(const String& filepath)
	{
		return App::RenderDevice().CreateShader(filepath, ShaderMacros{});
	}
	
	Ref<Shader> Shader::Create(const String& filepath, const ShaderMacros& macros)
	{
		return App::RenderDevice().CreateShader(filepath, macros);
	}
}
