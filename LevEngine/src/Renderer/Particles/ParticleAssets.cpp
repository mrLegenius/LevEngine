#include "levpch.h"
#include "ParticleAssets.h"

#include "Assets/EngineAssets.h"
#include "Renderer/Shader/Shader.h"
#include "TextureLibrary.h"

namespace LevEngine
{
    Ref<Shader> ParticleShaders::Rendering()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("Particles/Rendering.hlsl"),
                                                   ShaderType::Vertex | ShaderType::Geometry | ShaderType::Pixel);

        return shader;
    }

    Ref<Shader> ParticleShaders::Simulation()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("Particles/Simulation.hlsl"),
                                                   ShaderType::Compute);

        return shader;
    }

    Ref<Shader> ParticleShaders::Emission()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("Particles/Emission.hlsl"),
                                                   ShaderType::Compute);

        return shader;
    }

    Ref<Shader> ParticleShaders::BitonicSort()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader =
            Shader::Create(GetShaderPath("Particles/BitonicSort.hlsl"), ShaderType::Compute);

        return shader;
    }

    Ref<Shader> ParticleShaders::BitonicTranspose()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("Particles/BitonicTranspose.hlsl"),
                                                   ShaderType::Compute);

        return shader;
    }
    
    Ref<Texture> ParticleTextures::Default()
    {
        static auto texture = TextureLibrary::GetTexture(ToString(EngineResourcesRoot / "Textures" / "particle.png"));
        return texture;
    }
}
