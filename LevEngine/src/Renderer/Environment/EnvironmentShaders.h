#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
    class Shader;

    struct LEV_API EnvironmentShaders
    {
        static const Ref<Shader>& EquirectangularToCubemap();
        static const Ref<Shader>& CubemapConvolution();
        static const Ref<Shader>& EnvironmentPreFiltering();
        static const Ref<Shader>& BRDFIntegration();
        static const Ref<Shader>& Render();
        static const Ref<Shader>& CubemapRender();
    };
}
