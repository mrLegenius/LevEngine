#include "levpch.h"
#include "EngineAssets.h"

#include "TextureLibrary.h"
#include "Renderer/Shader.h"

namespace LevEngine
{
    String GetShaderPath(const String& name) { return ToString(EngineResourcesRoot / "Shaders" / name.c_str()); }
    static String GetIconsPath(const String& name) { return ToString(EngineResourcesRoot / "Icons" / name.c_str()); }

    static auto GetIcon(const String& name) { return TextureLibrary::GetTexture(GetIconsPath(name)); }

    Ref<Texture> Icons::Play(){ return GetIcon("PlayButton.png"); }
    Ref<Texture> Icons::Stop(){ return GetIcon("StopButton.png"); }
    Ref<Texture> Icons::Rotate(){ return GetIcon("Rotate.png"); } 
    Ref<Texture> Icons::Scale(){ return GetIcon("Scale.png"); } 
    Ref<Texture> Icons::Select(){ return GetIcon("Select.png"); } 
    Ref<Texture> Icons::Translate(){ return GetIcon("Translate.png"); }
    
    Ref<Texture> Icons::Directory(){ return GetIcon("Assets\\DirectoryIcon.png"); } 
    Ref<Texture> Icons::File(){ return GetIcon("Assets\\FileIcon.png"); } 
    Ref<Texture> Icons::Material(){ return GetIcon("Assets\\MaterialIcon.png"); } 
    Ref<Texture> Icons::Mesh(){ return GetIcon("Assets\\MeshIcon.png"); } 
    Ref<Texture> Icons::Skybox(){ return GetIcon("Assets\\SkyboxIcon.png"); }

    Ref<Shader> ShaderAssets::Debug()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("Debug.hlsl"));
        return shader;
    }

    Ref<Shader> ShaderAssets::ForwardPBR()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("ForwardPBR.hlsl"));
        return shader;
    }

    Ref<Shader> ShaderAssets::Lit()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("Lit.hlsl"));
        return shader;
    }

    Ref<Shader> ShaderAssets::Unlit()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("Unlit.hlsl"));

        return shader;
    }

    Ref<Shader> ShaderAssets::ShadowPass()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("ShadowPass.hlsl"));

        return shader;
    }

    Ref<Shader> ShaderAssets::CascadeShadowPass()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("CascadeShadowPass.hlsl"),
                                                   ShaderType::Vertex | ShaderType::Geometry);

        return shader;
    }

    Ref<Shader> ShaderAssets::GBufferPass()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("DeferredOpaquePBR.hlsl"));

        return shader;
    }

    Ref<Shader> ShaderAssets::DeferredVertexOnly()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("DeferredLightningPassPBR.hlsl"),
                                                   ShaderType::Vertex);

        return shader;
    }

    Ref<Shader> ShaderAssets::DeferredPointLight()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("DeferredLightningPassPBR.hlsl"));

        return shader;
    }

    Ref<Shader> ShaderAssets::DeferredQuadRender()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("ForwardQuadRender.hlsl"));

        return shader;
    }

    Ref<Shader> ShaderAssets::Luminance()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("PostProcessing/Luminance.hlsl"));
        return shader;
    }

    Ref<Shader> ShaderAssets::LuminanceAdaptation()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("PostProcessing/LuminanceAdaptation.hlsl"));
        return shader;
    }

    Ref<Shader> ShaderAssets::Tonemapping()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("PostProcessing/Tonemapping.hlsl"));
        return shader;
    }

    Ref<Shader> ShaderAssets::Vignette()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("PostProcessing/Vignette.hlsl"));
        return shader;
    }

    Ref<Shader> ShaderAssets::Bloom()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("PostProcessing/Bloom.hlsl"));
        return shader;
    }

    Ref<Shader> ShaderAssets::BloomBlurH()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("PostProcessing/BloomBlurH.hlsl"));
        return shader;
    }

    Ref<Shader> ShaderAssets::BloomBlurV()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("PostProcessing/BloomBlurV.hlsl"));
        return shader;
    }

    Ref<Shader> ShaderAssets::Scale()
    {
        LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader = Shader::Create(GetShaderPath("PostProcessing/Scale.hlsl"));
        return shader;
    }
}
