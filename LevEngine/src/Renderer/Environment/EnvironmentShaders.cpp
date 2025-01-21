#include "levpch.h"
#include "EnvironmentShaders.h"

#include "Assets/EngineAssets.h"
#include "Assets/ShaderLibrary.h"
#include "Renderer/Shader/Shader.h"

namespace LevEngine
{
    const Ref<Shader>& EnvironmentShaders::EquirectangularToCubemap()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("Environment/EquirectangularToCubemap.hlsl"));
    }

    const Ref<Shader>& EnvironmentShaders::CubemapConvolution()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("Environment/CubemapConvolution.hlsl"));
    }

    const Ref<Shader>& EnvironmentShaders::EnvironmentPreFiltering()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("Environment/PreFiltering.hlsl"));
    }

    const Ref<Shader>& EnvironmentShaders::BRDFIntegration()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("Environment/BRDFIntegration.hlsl"));
    }

    const Ref<Shader>& EnvironmentShaders::Render()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("Environment/Render.hlsl"));
    }

    const Ref<Shader>& EnvironmentShaders::CubemapRender()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("Environment/CubemapRender.hlsl"));
    }
}
