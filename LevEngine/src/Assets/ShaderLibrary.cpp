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

    const Ref<Shader>& ShaderLibrary::GetOrAddShader(const Path& path, const ShaderMacros& shaderMacros)
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
