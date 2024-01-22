#include "levpch.h"
#include "EngineAssets.h"

#include "TextureLibrary.h"
#include "Renderer/Shader.h"

namespace LevEngine
{
    String GetShaderPath(const String& name) { return ToString(EngineResourcesRoot / "Shaders" / name.c_str()); }
    static String GetIconsPath(const String& name) { return ToString(EngineResourcesRoot / "Icons" / name.c_str()); }

    static auto GetIcon(const String& name) { return TextureLibrary::GetTexture(GetIconsPath(name)); }

    Ref<Texture> Icons::Play() { return GetIcon("PlayButton.png"); }
    Ref<Texture> Icons::Stop() { return GetIcon("StopButton.png"); }

    Ref<Texture> Icons::Directory() { return GetIcon("Assets\\DirectoryIcon.png"); }
    Ref<Texture> Icons::File() { return GetIcon("Assets\\FileIcon.png"); }
    Ref<Texture> Icons::Material() { return GetIcon("Assets\\MaterialIcon.png"); }
    Ref<Texture> Icons::Mesh() { return GetIcon("Assets\\MeshIcon.png"); }
    Ref<Texture> Icons::Skybox() { return GetIcon("Assets\\SkyboxIcon.png"); }

    Ref<Shader> ShaderAssets::DebugShape()
    {
        static Ref<Shader> shader = Shader::Create(GetShaderPath("DebugRender\\DebugShape.hlsl"));
        return shader;
    }

    Ref<Shader> ShaderAssets::DebugLine()
    {
        static Ref<Shader> shader = Shader::Create(GetShaderPath("DebugRender\\DebugLine.hlsl"));
        return shader;
    }

    Ref<Shader> ShaderAssets::ForwardPBR()
    {
        static Ref<Shader> shader = Shader::Create(GetShaderPath("ForwardPBR.hlsl"));
        return shader;
    }

    Ref<Shader> ShaderAssets::Lit()
    {
        static Ref<Shader> shader = Shader::Create(GetShaderPath("Lit.hlsl"));
        return shader;
    }

    Ref<Shader> ShaderAssets::Unlit()
    {
        static Ref<Shader> shader = Shader::Create(GetShaderPath("Unlit.hlsl"));

        return shader;
    }

    Ref<Shader> ShaderAssets::ShadowPass()
    {
        static Ref<Shader> shader = Shader::Create(GetShaderPath("ShadowPass.hlsl"));

        return shader;
    }

    Ref<Shader> ShaderAssets::CascadeShadowPass()
    {
        static Ref<Shader> shader = Shader::Create(GetShaderPath("CascadeShadowPass.hlsl"),
                                                   ShaderType::Vertex | ShaderType::Geometry);

        return shader;
    }

    Ref<Shader> ShaderAssets::GBufferPass()
    {
        static Ref<Shader> shader = Shader::Create(GetShaderPath("DeferredOpaquePBR.hlsl"));

        return shader;
    }

    Ref<Shader> ShaderAssets::DeferredVertexOnly()
    {
        static Ref<Shader> shader = Shader::Create(GetShaderPath("DeferredLightningPassPBR.hlsl"),
                                                   ShaderType::Vertex);

        return shader;
    }

    Ref<Shader> ShaderAssets::DeferredPointLight()
    {
        static Ref<Shader> shader = Shader::Create(GetShaderPath("DeferredLightningPassPBR.hlsl"));

        return shader;
    }

    Ref<Shader> ShaderAssets::DeferredQuadRender()
    {
        static Ref<Shader> shader = Shader::Create(GetShaderPath("ForwardQuadRender.hlsl"));

        return shader;
    }

    Ref<Shader> ShaderAssets::Luminance()
    {
        static Ref<Shader> shader = Shader::Create(GetShaderPath("PostProcessing/Luminance.hlsl"));
        return shader;
    }

    Ref<Shader> ShaderAssets::LuminanceAdaptation()
    {
        static Ref<Shader> shader = Shader::Create(GetShaderPath("PostProcessing/LuminanceAdaptation.hlsl"));
        return shader;
    }

    Ref<Shader> ShaderAssets::Tonemapping()
    {
        static Ref<Shader> shader = Shader::Create(GetShaderPath("PostProcessing/Tonemapping.hlsl"));
        return shader;
    }

    Ref<Shader> ShaderAssets::Vignette()
    {
        static Ref<Shader> shader = Shader::Create(GetShaderPath("PostProcessing/Vignette.hlsl"));
        return shader;
    }

    Ref<Shader> ShaderAssets::Bloom()
    {
        static Ref<Shader> shader = Shader::Create(GetShaderPath("PostProcessing/Bloom.hlsl"));
        return shader;
    }

    Ref<Shader> ShaderAssets::BloomBlurH()
    {
        static Ref<Shader> shader = Shader::Create(GetShaderPath("PostProcessing/BloomBlurH.hlsl"));
        return shader;
    }

    Ref<Shader> ShaderAssets::BloomBlurV()
    {
        static Ref<Shader> shader = Shader::Create(GetShaderPath("PostProcessing/BloomBlurV.hlsl"));
        return shader;
    }

    Ref<Shader> ShaderAssets::Scale()
    {
        static Ref<Shader> shader = Shader::Create(GetShaderPath("PostProcessing/Scale.hlsl"));
        return shader;
    }
}
