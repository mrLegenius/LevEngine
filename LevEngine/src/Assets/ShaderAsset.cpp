#include "levpch.h"
#include "ShaderAsset.h"

#include "EngineAssets.h"
#include "ShaderLibrary.h"

namespace LevEngine
{
    static const Path DefaultTemplatePath = EngineResourcesRoot / "Shaders/Templates/CustomShader.hlsl";

    ShaderAsset::ShaderAsset(const Path& path, const UUID& uuid) : Asset(path, uuid) { }

    ShaderAsset::ShaderAsset(const Path& path, const UUID& uuid, const CreateFrom source) : Asset(path, uuid)
    {
        switch (source)
        {
        case CreateFrom::Template:
            copy(DefaultTemplatePath, path, std::filesystem::copy_options::overwrite_existing);
            break;
        default:
            std::ofstream fout(m_Path);
        }
    }

    ShaderAsset::ShaderAsset(const Path& path, const UUID& uuid, const ShaderMacros& shaderMacros)
        : Asset(path, uuid), m_ShaderMacros(shaderMacros), m_GenerateMeta(false)
    { }

    Ref<Shader> ShaderAsset::GetShader() const
    {
        //<--- Project assets do not own a shader, they point at the library's ---<<
        if (m_GenerateMeta)
            return ShaderLibrary::GetOrAddShader(m_Path);

        return m_Shader;
    }

    void ShaderAsset::DeserializeData(const YAML::Node& node)
    {
        //<--- Project shaders are compiled on demand and reloaded by the ShaderLibrary ---<<
        if (m_GenerateMeta) return;

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
