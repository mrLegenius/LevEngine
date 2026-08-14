#include "levpch.h"
#include "PlanetShaders.h"

#include "Assets/EngineAssets.h"
#include "Assets/ShaderLibrary.h"
#include "Renderer/Shader/Shader.h"

namespace LevEngine
{
	Ref<Shader> PlanetShaders::Surface()
	{
		return ShaderLibrary::GetOrAddShader(GetShaderPath("Planet/PlanetSurface.hlsl"));
	}

	Ref<Shader> PlanetShaders::SurfaceDeferred()
	{
		ShaderMacros macros;
		macros.emplace("LEV_DEFERRED", "1");

		return ShaderLibrary::GetOrAddShader(GetShaderPath("Planet/PlanetSurface.hlsl"), macros);
	}

	Ref<Shader> PlanetShaders::Ocean()
	{
		return ShaderLibrary::GetOrAddShader(GetShaderPath("Planet/PlanetOcean.hlsl"));
	}
}
