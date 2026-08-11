#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
    class Texture;
    class Shader;
    
    struct LEV_API ParticleShaders
    {
        static Ref<Shader> Rendering();
        static Ref<Shader> Simulation();
        static Ref<Shader> Emission();
        static Ref<Shader> BitonicSort();
        static Ref<Shader> BitonicTranspose();
    };

    struct LEV_API ParticleTextures
    {
        static Ref<Texture> Default();
    };
}
