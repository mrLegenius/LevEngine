#include "levpch.h"
#include "EngineAssets.h"

#include "ShaderLibrary.h"
#include "TextureLibrary.h"
#include "Renderer/Shader/Shader.h"

namespace LevEngine
{
    Path GetShaderPath(const String& name) { return EngineResourcesRoot / "Shaders" / name.c_str(); }
    static String GetIconsPath(const String& name) { return ToString(EngineResourcesRoot / "Icons" / name.c_str()); }

    static auto GetIcon(const String& name) { return TextureLibrary::GetTexture(GetIconsPath(name)); }

    Ref<Texture> Icons::Play() { return GetIcon("PlayButton.png"); }
    Ref<Texture> Icons::Stop() { return GetIcon("StopButton.png"); }

    Ref<Texture> Icons::Directory() { return GetIcon("Assets\\DirectoryIcon.png"); }
    Ref<Texture> Icons::File() { return GetIcon("Assets\\FileIcon.png"); }
    Ref<Texture> Icons::Material() { return GetIcon("Assets\\MaterialIcon.png"); }
    Ref<Texture> Icons::Mesh() { return GetIcon("Assets\\MeshIcon.png"); }
    Ref<Texture> Icons::Model() { return GetIcon("Assets\\ModelIcon.png"); }
    Ref<Texture> Icons::Skybox() { return GetIcon("Assets\\SkyboxIcon.png"); }

    Ref<Shader> ShaderAssets::DebugShape()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("DebugRender\\DebugShape.hlsl"));
    }

    Ref<Shader> ShaderAssets::DebugLine()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("DebugRender\\DebugLine.hlsl"));
    }

    Ref<Shader> ShaderAssets::ForwardPBR()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("ForwardPBR.hlsl"));
    }

    Ref<Shader> ShaderAssets::Lit()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("Lit.hlsl"));
    }

    Ref<Shader> ShaderAssets::Unlit()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("Unlit.hlsl"));
    }

    Ref<Shader> ShaderAssets::ShadowPass()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("ShadowPass.hlsl"));
    }

    Ref<Shader> ShaderAssets::CascadeShadowPass()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("CascadeShadowPass.hlsl"));
    }

    Ref<Shader> ShaderAssets::CascadeShadowPassWithAnimations()
    {
        ShaderMacros macros;
        macros.emplace("WITH_ANIMATIONS", "1");
        
        return ShaderLibrary::GetOrAddShader(GetShaderPath("CascadeShadowPass.hlsl"), macros);
    }

    Ref<Shader> ShaderAssets::GBufferPass()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("DeferredOpaquePBR.hlsl"));
    }

    Ref<Shader> ShaderAssets::GBufferPassWithAnimations()
    {
        ShaderMacros macros;
        macros.emplace("WITH_ANIMATIONS", "1");

        return ShaderLibrary::GetOrAddShader(GetShaderPath("DeferredOpaquePBR.hlsl"), macros);
    }

    Ref<Shader> ShaderAssets::DeferredVertexOnly()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("DeferredLightningPassPBR_VS.hlsl"));
    }

    Ref<Shader> ShaderAssets::DeferredPointLight()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("DeferredLightningPassPBR.hlsl"));
    }

    Ref<Shader> ShaderAssets::DeferredQuadRender()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("ForwardQuadRender.hlsl"));
    }

    Ref<Shader> ShaderAssets::Luminance()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("PostProcessing/Luminance.hlsl"));
    }

    Ref<Shader> ShaderAssets::LuminanceAdaptation()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("PostProcessing/LuminanceAdaptation.hlsl"));
    }

    Ref<Shader> ShaderAssets::Tonemapping()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("PostProcessing/Tonemapping.hlsl"));
    }

    Ref<Shader> ShaderAssets::Vignette()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("PostProcessing/Vignette.hlsl"));
    }

    Ref<Shader> ShaderAssets::Bloom()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("PostProcessing/Bloom.hlsl"));
    }

    Ref<Shader> ShaderAssets::BloomBlurH()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("PostProcessing/BloomBlurH.hlsl"));
    }

    Ref<Shader> ShaderAssets::BloomBlurV()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("PostProcessing/BloomBlurV.hlsl"));
    }

    Ref<Shader> ShaderAssets::Scale()
    {
        return ShaderLibrary::GetOrAddShader(GetShaderPath("PostProcessing/Scale.hlsl"));
    }
}
