#include "levpch.h"
#include "EnvironmentShaders.h"

#include "Assets/EngineAssets.h"
#include "Renderer/Shader.h"

namespace LevEngine
{
    Ref<Shader> EnvironmentShaders::EquirectangularToCubemap()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(
            GetShaderPath("Environment/EquirectangularToCubemap.hlsl"),
            ShaderType::Pixel);
        return shader;
    }

    Ref<Shader> EnvironmentShaders::CubemapConvolution()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(
            GetShaderPath("Environment/CubemapConvolution.hlsl"),
            ShaderType::Pixel);
        return shader;
    }

    Ref<Shader> EnvironmentShaders::EnvironmentPreFiltering()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(
            GetShaderPath("Environment/PreFiltering.hlsl"),
            ShaderType::Pixel);
        return shader;
    }

    Ref<Shader> EnvironmentShaders::BRDFIntegration()
    {
        LEV_PROFILE_FUNCTION();
        
        static Ref<Shader> shader = Shader::Create(
            GetShaderPath("Environment/BRDFIntegration.hlsl"));
        return shader;
    }

    Ref<Shader> EnvironmentShaders::Render()
    {
        LEV_PROFILE_FUNCTION();
        
        static Ref<Shader> shader = Shader::Create(
            GetShaderPath("Environment/Render.hlsl"));
        return shader;
    }

    Ref<Shader> EnvironmentShaders::CubemapRender()
    {
        LEV_PROFILE_FUNCTION();
        
        static Ref<Shader> shader = Shader::Create(
            GetShaderPath("Environment/CubemapRender.hlsl"),
            ShaderType::Vertex | ShaderType::Geometry);
        return shader;
    }
}
