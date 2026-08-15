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

        // Recompiles every cached permutation of a file whether or not it changed on disk, and
        // returns the files it touched. A caller that wants to know the result reads
        // ShaderDiagnostics around it.
        //
        // An empty path means every shader the library holds. Matching is by suffix, so
        // "Environment/AtmosphereSky.hlsl" and the absolute path both find the same shader.
        static Vector<Path> ForceReimport(const Path& path);

        //<--- Every shader file the library has compiled something from ---<<
        static Vector<Path> GetLoadedPaths();

        //Releases every cached shader. Has to be called while the render device is still alive,
        //see TextureLibrary::Shutdown
        static void Shutdown();

    private:
        //<--- Shaders compiled from the same file with different macros are different shaders ---<<
        static String GetKey(const Path& path, const ShaderMacros& shaderMacros);

	    static inline UnorderedMap<String, Ref<ShaderAsset>> s_ShaderAssets;
    };

}
