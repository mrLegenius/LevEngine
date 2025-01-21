#include "levpch.h"
#include "ShaderAsset.h"

namespace LevEngine
{
    ShaderAsset::ShaderAsset(const Path& path, const UUID& uuid, const ShaderMacros& shaderMacros) : Asset(path, uuid), m_ShaderMacros(shaderMacros)
    { }

    ShaderAsset::ShaderAsset(const Path& path, const UUID& uuid) : ShaderAsset(path, uuid, ShaderMacros{})
    { }

    void ShaderAsset::DeserializeData(const YAML::Node& node)
    {
        if (m_Shader)
        {
            m_Shader->Reload();
        }
        else
        {
            m_Shader = Shader::Create(m_Path.string().c_str(), m_ShaderMacros);
        }
    }
}
