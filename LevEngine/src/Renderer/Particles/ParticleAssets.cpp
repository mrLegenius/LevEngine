#include "levpch.h"
#include "ParticleAssets.h"

#include "Assets/EngineAssets.h"
#include "Renderer/Shader/Shader.h"
#include "TextureLibrary.h"
#include "Assets/ShaderLibrary.h"

namespace LevEngine
{
    Ref<Shader> ParticleShaders::Rendering()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("Particles/Rendering.hlsl"));
    }

    Ref<Shader> ParticleShaders::Simulation()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("Particles/Simulation.hlsl"));
    }
    
    Ref<Shader> ParticleShaders::Emission()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("Particles/Emission.hlsl"));
    }

    Ref<Shader> ParticleShaders::BitonicSort()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("Particles/BitonicSort.hlsl"));
    }

    Ref<Shader> ParticleShaders::BitonicTranspose()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("Particles/BitonicTranspose.hlsl"));
    }
    
    Ref<Texture> ParticleTextures::Default()
    {
        static auto texture = TextureLibrary::GetTexture(ToString(EngineResourcesRoot / "Textures" / "particle.png"));
        return texture;
    }
}
