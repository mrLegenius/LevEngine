#include "levpch.h"
#include "EnvironmentShaders.h"

#include "Assets.h"
#include "Renderer/Shader.h"

namespace LevEngine
{
    Ref<Shader> EnvironmentShaders::EquirectangularToCubemap()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("EquirectangularToCubemap.hlsl"),
                                                   ShaderType::Pixel | ShaderType::Vertex | ShaderType::Geometry);
        return shader;
    }

    Ref<Shader> EnvironmentShaders::CubemapConvolution()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("CubemapConvolution.hlsl"),
                                                   ShaderType::Pixel | ShaderType::Vertex | ShaderType::Geometry);
        return shader;
    }

    Ref<Shader> EnvironmentShaders::EnvironmentPreFiltering()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("Environment/PreFiltering.hlsl"),
                                                   ShaderType::Pixel | ShaderType::Vertex | ShaderType::Geometry);
        return shader;
    }

    Ref<Shader> EnvironmentShaders::BRDFIntegration()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("Environment/BRDFIntegration.hlsl"));
        return shader;
    }
}
