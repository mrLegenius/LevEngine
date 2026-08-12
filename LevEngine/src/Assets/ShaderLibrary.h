#pragma once

#include "Kernel/Core.h"
#include "Renderer/Shader/Shader.h"

namespace LevEngine
{
    class ShaderAsset;

    class LEV_API ShaderLibrary
    {
    public:
        static Ref<Shader> GetOrAddShader(const Path& path, const ShaderMacros& shaderMacros = ShaderMacros{});
        static void ReimportChangedAssets();

    private:
        //<--- Shaders compiled from the same file with different macros are different shaders ---<<
        static String GetKey(const Path& path, const ShaderMacros& shaderMacros);

	    static inline UnorderedMap<String, Ref<ShaderAsset>> s_ShaderAssets;
    };

}
