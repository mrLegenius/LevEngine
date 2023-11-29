#pragma once

namespace LevEngine
{
    class Texture;
    class Shader;
    
    struct ParticleShaders
    {
        static Ref<Shader> Rendering();
        static Ref<Shader> Simulation();
        static Ref<Shader> Emission();
        static Ref<Shader> BitonicSort();
        static Ref<Shader> BitonicTranspose();
    };

    struct ParticleTextures
    {
        static Ref<Texture> Default();
    };
}
