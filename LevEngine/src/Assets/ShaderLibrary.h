#pragma once
#include "Renderer/Shader/Shader.h"

namespace LevEngine
{
    class ShaderAsset;

    class ShaderLibrary
    {
    public:
        static const Ref<Shader>& GetOrAddShader(const Path& path, const ShaderMacros& shaderMacros = ShaderMacros{});
        static void ReimportChangedAssets();

    private:
	    static inline UnorderedMap<Path, Ref<ShaderAsset>> m_ShaderAssets;
    };

}
