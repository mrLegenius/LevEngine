#pragma once

namespace LevEngine
{
    inline static Path EngineResourcesRoot = Path("LevResources");
    
    inline String GetShaderPath(const String& name);
    
    class Shader;
    class Texture;

    struct Icons
    {
        static Ref<Texture> Play();
        static Ref<Texture> Stop();

        static Ref<Texture> Directory();
        static Ref<Texture> File();
        static Ref<Texture> Material();
        static Ref<Texture> Mesh();
        static Ref<Texture> Model();
        static Ref<Texture> Skybox();
    };

    struct ShaderAssets
    {
        static Ref<Shader> DebugShape();
        static Ref<Shader> DebugLine();
        
        static Ref<Shader> Lit();
        static Ref<Shader> Unlit();

        static Ref<Shader> ShadowPass();
        static Ref<Shader> CascadeShadowPass();
        static Ref<Shader> CascadeShadowPassWithAnimations();

        static Ref<Shader> ForwardPBR();
        
        static Ref<Shader> GBufferPass();
        static Ref<Shader> GBufferPassWithAnimations();
        static Ref<Shader> DeferredVertexOnly();
        static Ref<Shader> DeferredPointLight();
        static Ref<Shader> DeferredQuadRender();
        
        static Ref<Shader> Luminance();
        static Ref<Shader> LuminanceAdaptation();
        static Ref<Shader> Tonemapping();
        static Ref<Shader> Vignette();
        static Ref<Shader> Bloom();
        static Ref<Shader> BloomBlurH();
        static Ref<Shader> BloomBlurV();
        static Ref<Shader> Scale();
    };
}
