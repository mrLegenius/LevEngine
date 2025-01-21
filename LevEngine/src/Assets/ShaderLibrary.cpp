#include "levpch.h"
#include "ShaderLibrary.h"

#include "ShaderAsset.h"

namespace LevEngine
{
    const Ref<Shader>& ShaderLibrary::GetOrAddShader(const Path& path, const ShaderMacros& shaderMacros)
    {
        LEV_PROFILE_FUNCTION();

        auto it = m_ShaderAssets.find(path);

        Ref<ShaderAsset> shaderAsset;
        if (it == m_ShaderAssets.end())
        {
            shaderAsset = CreateRef<ShaderAsset>(path, UUID(), shaderMacros);
            m_ShaderAssets[path] = shaderAsset;
            shaderAsset->Serialize();
            shaderAsset->Deserialize();
        }
        else
        {
            shaderAsset = it->second;
        }

        return shaderAsset->GetShader();
    }

    void ShaderLibrary::ReimportChangedAssets()
    {
        for (auto& it : m_ShaderAssets)
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
