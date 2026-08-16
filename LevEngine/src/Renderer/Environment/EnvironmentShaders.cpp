#include "levpch.h"
#include "EnvironmentShaders.h"

#include "Assets/EngineAssets.h"
#include "Assets/ShaderLibrary.h"
#include "Renderer/Shader/Shader.h"

namespace LevEngine
{
    Ref<Shader> EnvironmentShaders::EquirectangularToCubemap()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("Environment/EquirectangularToCubemap.hlsl"));
    }

    Ref<Shader> EnvironmentShaders::CubemapConvolution()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("Environment/CubemapConvolution.hlsl"));
    }

    Ref<Shader> EnvironmentShaders::EnvironmentPreFiltering()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("Environment/PreFiltering.hlsl"));
    }

    Ref<Shader> EnvironmentShaders::BRDFIntegration()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("Environment/BRDFIntegration.hlsl"));
    }

    Ref<Shader> EnvironmentShaders::Render()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("Environment/Render.hlsl"));
    }

    Ref<Shader> EnvironmentShaders::CubemapRender()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("Environment/CubemapRender.hlsl"));
    }

    Ref<Shader> EnvironmentShaders::PlanetAtmosphere()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("Environment/PlanetAtmosphere.hlsl"));
    }

    Ref<Shader> EnvironmentShaders::AtmosphereSky()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("Environment/AtmosphereSky.hlsl"));
    }

    Ref<Shader> EnvironmentShaders::AtmosphereCubemap()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("Environment/AtmosphereCubemap.hlsl"));
    }
}
