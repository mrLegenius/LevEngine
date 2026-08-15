#include "levpch.h"
#include "ShaderLibrary.h"

#include "ShaderAsset.h"

namespace LevEngine
{
    String ShaderLibrary::GetKey(const Path& path, const ShaderMacros& shaderMacros)
    {
        String key = ToString(path);

        //<--- ShaderMacros is an ordered map, so the key is stable ---<<
        for (const auto& [name, definition] : shaderMacros)
        {
            key.append("|");
            key.append(name);
            key.append("=");
            key.append(definition);
        }

        return key;
    }

    Ref<Shader> ShaderLibrary::GetOrAddShader(const Path& path, const ShaderMacros& shaderMacros)
    {
        LEV_PROFILE_FUNCTION();

        const auto key = GetKey(path, shaderMacros);
        const auto it = s_ShaderAssets.find(key);

        if (it != s_ShaderAssets.end())
            return it->second->GetShader();

        auto shaderAsset = CreateRef<ShaderAsset>(path, UUID(), shaderMacros);
        s_ShaderAssets[key] = shaderAsset;
        shaderAsset->Serialize();
        shaderAsset->Deserialize();

        return shaderAsset->GetShader();
    }

    void ShaderLibrary::Shutdown()
    {
        s_ShaderAssets.clear();
    }

    namespace
    {
        String NormalizePath(const Path& path)
        {
            String text = ToString(path);

            for (auto& character : text)
            {
                character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));

                if (character == '\\')
                    character = '/';
            }

            return text;
        }

        bool PathMatches(const Path& assetPath, const String& wanted)
        {
            if (wanted.empty()) return true;

            const auto normalized = NormalizePath(assetPath);

            //<--- Suffix, so a relative path finds the shader an absolute one would ---<<
            return normalized.size() >= wanted.size()
                && normalized.compare(normalized.size() - wanted.size(), wanted.size(), wanted) == 0;
        }
    }

    Vector<Path> ShaderLibrary::ForceReimport(const Path& path)
    {
        const auto wanted = NormalizePath(path);

        Vector<Path> reimported;

        for (auto& [key, shaderAsset] : s_ShaderAssets)
        {
            if (!PathMatches(shaderAsset->GetPath(), wanted)) continue;

            shaderAsset->Clear();
            shaderAsset->Deserialize(true);

            reimported.push_back(shaderAsset->GetPath());
        }

        return reimported;
    }

    Vector<Path> ShaderLibrary::GetLoadedPaths()
    {
        Vector<Path> paths;

        for (const auto& [key, shaderAsset] : s_ShaderAssets)
        {
            const auto& assetPath = shaderAsset->GetPath();

            //<--- One entry per file, not per permutation ---<<
            if (eastl::find(paths.begin(), paths.end(), assetPath) == paths.end())
                paths.push_back(assetPath);
        }

        return paths;
    }

    void ShaderLibrary::ReimportChangedAssets()
    {
        for (auto& it : s_ShaderAssets)
        {
            auto& shaderAsset = it.second;

            if (shaderAsset->IsReimportNeeded())
            {
                shaderAsset->Clear();
                shaderAsset->Deserialize(true);
            }
        }
    }
}
