#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
    inline static Path EngineResourcesRoot = Path("LevResources");
    
    inline Path GetShaderPath(const String& name);
    
    class Shader;
    class Texture;

    struct LEV_API Icons
    {
        static Ref<Texture> Logo();

        static Ref<Texture> Play();
        static Ref<Texture> Stop();

        static Ref<Texture> Directory();
        static Ref<Texture> File();
        static Ref<Texture> Material();
        static Ref<Texture> Mesh();
        static Ref<Texture> Model();
        static Ref<Texture> Skybox();
    };

    struct LEV_API ShaderAssets
    {
        static Ref<Shader> DebugShape();
        static Ref<Shader> DebugLine();
        
        static Ref<Shader> Unlit();

        static Ref<Shader> CascadeShadowPass();
        static Ref<Shader> CascadeShadowPassWithAnimations();
        static Ref<Shader> CascadeShadowPassInstanced();

        static Ref<Shader> ForwardPBR();
        static Ref<Shader> ForwardPBRInstanced();

        static Ref<Shader> GBufferPass();
        static Ref<Shader> GBufferPassWithAnimations();
        static Ref<Shader> GBufferPassInstanced();
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
