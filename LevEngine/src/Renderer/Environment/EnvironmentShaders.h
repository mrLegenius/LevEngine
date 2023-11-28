#pragma once

namespace LevEngine
{
    class Shader;

    struct EnvironmentShaders
    {
        static Ref<Shader> EquirectangularToCubemap();
        static Ref<Shader> CubemapConvolution();
        static Ref<Shader> EnvironmentPreFiltering();
        static Ref<Shader> BRDFIntegration();
        static Ref<Shader> Render();
        static Ref<Shader> CubemapRender();
    };
}
