#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
    class Shader;

    struct LEV_API EnvironmentShaders
    {
        static Ref<Shader> EquirectangularToCubemap();
        static Ref<Shader> CubemapConvolution();
        static Ref<Shader> EnvironmentPreFiltering();
        static Ref<Shader> BRDFIntegration();
        static Ref<Shader> Render();
        static Ref<Shader> CubemapRender();

        static Ref<Shader> AtmosphereSky();
        static Ref<Shader> AtmosphereCubemap();

        //<--- The air in front of the scene, as opposed to the sky behind it ---<<
        static Ref<Shader> PlanetAtmosphere();
    };
}
